#include "TcpServer.h"
#include "TcpSession.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

TcpServerRef
TcpServer::create_instance(asio::io_service & io)
{
    return TcpServerRef(new TcpServer(io))->shared_from_this();
}

TcpServer::TcpServer(asio::io_service & io) : Server(io),
                                              m_accept_handler(nullptr),
                                              m_cancel_handler(nullptr)
{

}

TcpServer::~TcpServer()
{
    cancel();
}

void
TcpServer::accept(uint16_t port)
{
    if (m_acceptor)
    {
        m_acceptor.reset();
    }

    m_acceptor = TcpAcceptorRef(new asio::ip::tcp::acceptor(m_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)));

    listen();
}

void
TcpServer::listen()
{
    if (!m_acceptor->is_open())
    {
        return;
    }

    TcpSessionRef session = TcpSession::create_instance(m_service);

    m_acceptor->async_accept(*session->m_socket, m_strand.wrap(std::bind(&TcpServer::on_accept,
                                                                         shared_from_this(),
                                                                         session,
                                                                         std::placeholders::_1)));
}

void
TcpServer::cancel()
{
    if (m_acceptor)
    {
        asio::error_code err;
        m_acceptor->cancel(err);

        if (check_error(err, 0))
        {
            if (m_cancel_handler != nullptr)
            {
                m_cancel_handler();
            }
        }

        m_acceptor->close(err);

        check_error(err, 0);
    }
}

void
TcpServer::on_accept(TcpSessionRef session, const asio::error_code & err)
{
    if (!check_error(err, 0))
    {
        return;
    }

    if (m_accept_handler != nullptr)
    {
        m_accept_handler(session);
    }

    listen();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
