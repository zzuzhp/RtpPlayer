#include "TcpClient.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

TcpClientRef
TcpClient::create_instance(asio::io_service & io)
{
    return TcpClientRef(new TcpClient(io))->shared_from_this();
}

TcpClient::TcpClient(asio::io_service & io) : Client(io),
                                              m_connect_handler(nullptr)
{

}

TcpClient::~TcpClient()
{

}

void
TcpClient::connect(const std::string host, uint16_t port)
{
    std::ostringstream stm; ///< workaround of std::to_string(...)
    stm << port;

    connect(host, stm.str());
}

void
TcpClient::connect(const std::string host, const std::string protocol)
{
    asio::ip::tcp::resolver::query query(host, protocol);

    m_resolver = TcpResolverRef(new asio::ip::tcp::resolver(m_strand.get_io_service()));

    m_resolver->async_resolve(query, m_strand.wrap(std::bind(&TcpClient::on_resolve,
                                                             shared_from_this(),
                                                             std::placeholders::_1,
                                                             std::placeholders::_2)));
}

void
TcpClient::on_connect(TcpSessionRef session, const asio::error_code & err)
{
    if (!check_error(err, 0))
    {
        return;
    }

    if (m_connect_handler != nullptr)
    {
        m_connect_handler(session);
    }
}

void
TcpClient::on_resolve(const asio::error_code & err, asio::ip::tcp::resolver::iterator iter)
{
    if (!check_error(err, 0))
    {
        return;
    }

    if (m_resolve_handler != nullptr)
    {
        m_resolve_handler();
    }

    TcpSessionRef session(new TcpSession(m_service));
    asio::async_connect(*session->m_socket, iter, m_strand.wrap(std::bind(&TcpClient::on_connect,
                                                                          shared_from_this(),
                                                                          session,
                                                                          std::placeholders::_1)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
