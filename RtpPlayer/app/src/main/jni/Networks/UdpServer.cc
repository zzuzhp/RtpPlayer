#include "UdpServer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

UdpServerRef
UdpServer::create_instance(asio::io_service & io)
{
    return UdpServerRef(new UdpServer(io))->shared_from_this();
}

UdpServer::UdpServer(asio::io_service & io) : Server(io),
                                              m_accept_handler(nullptr)
{

}

UdpServer::~UdpServer()
{
    RP_FOOTPRINT
}

void
UdpServer::accept(uint16_t port)
{
    UdpSessionRef session = UdpSession::create_instance(m_service);

    asio::error_code err;
    session->m_socket->open(asio::ip::udp::v4(), err);

    if (!check_error(err, 0))
    {
        return;
    }

    session->m_socket->set_option(asio::socket_base::reuse_address(true));
    session->m_socket->bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), port), err);

    if (!check_error(err, 0))
    {
        return;
    }

    if (m_accept_handler)
    {
        m_accept_handler(session);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
