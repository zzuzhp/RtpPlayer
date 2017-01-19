#include "UdpClient.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

UdpClientRef
UdpClient::create_instance(asio::io_service & io)
{
    return UdpClientRef(new UdpClient(io))->shared_from_this();
}

UdpClient::UdpClient(asio::io_service & io) : Client(io),
                                              m_resolver(nullptr),
                                              m_connect_handler(nullptr)
{

}

UdpClient::~UdpClient()
{

}

void
UdpClient::connect(const std::string host, uint16_t port)
{
    std::ostringstream stm; ///< workaround of std::to_string(...)
    stm << port;

    connect(host, stm.str());
}

void
UdpClient::connect(const std::string host, const std::string protocol)
{
    asio::ip::udp::resolver::query query(host, protocol);

    m_resolver = UdpResolverRef(new asio::ip::udp::resolver(m_strand.get_io_service()));

    m_resolver->async_resolve(query, m_strand.wrap(std::bind(&UdpClient::on_resolve,
                                                             shared_from_this(),
                                                             std::placeholders::_1,
                                                             std::placeholders::_2)));
}

void
UdpClient::on_connect(UdpSessionRef session, const asio::error_code & err)
{
    if (!check_error(err, 0))
    {
        return;
    }

    if (m_connect_handler != nullptr)
    {
        session->m_socket->set_option(asio::socket_base::reuse_address(true));
        m_connect_handler(session);
    }
}

void
UdpClient::on_resolve(const asio::error_code & err, asio::ip::udp::resolver::iterator itr)
{
    if (!check_error(err, 0))
    {
        return;
    }

    if (m_resolve_handler != nullptr)
    {
        m_resolve_handler();
    }

    UdpSessionRef session = UdpSession::create_instance(m_service);

    asio::async_connect(*session->m_socket, itr, m_strand.wrap(std::bind(&UdpClient::on_connect,
                                                                         shared_from_this(),
                                                                         session,
                                                                         std::placeholders::_1)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
