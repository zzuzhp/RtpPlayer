#include "TcpSession.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

TcpSessionRef
TcpSession::create_instance(asio::io_service & io)
{
    return TcpSessionRef(new TcpSession(io))->shared_from_this();
}

TcpSession::TcpSession(asio::io_service & io) : Session(io),
                                                m_close(nullptr)
{
    m_socket = TcpSocketRef(new asio::ip::tcp::socket(io));
}

TcpSession::~TcpSession()
{
    close();
}

void
TcpSession::close()
{
    if (m_socket && m_socket->is_open())
    {
        asio::error_code err;
        m_socket->close(err);

        if (!check_error(err, 0))
        {
            return;
        }

        if (m_close != nullptr)
        {
            m_close();
        }
    }
}

void
TcpSession::on_close(const asio::error_code & err)
{
    if (!check_error(err, 0))
    {
        return;
    }

    if (m_close != nullptr)
    {
        m_close();
    }
}

void
TcpSession::read()
{
    asio::async_read(*m_socket,
                     m_response,
                     asio::transfer_at_least(1),
                     m_strand.wrap(std::bind(&TcpSession::on_read,
                                             shared_from_this(),
                                             std::placeholders::_1,
                                             std::placeholders::_2)));

    m_socket->set_option(asio::socket_base::reuse_address(true));
}

void
TcpSession::read(size_t size)
{
    m_socket->async_read_some(m_response.prepare(size),
                              m_strand.wrap(std::bind(&TcpSession::on_read,
                                                      shared_from_this(),
                                                      std::placeholders::_1,
                                                      std::placeholders::_2)));
}

void
TcpSession::write(const char * data, size_t size)
{
    std::ostream stream(&m_request);
    stream.write(data, size);

    asio::async_write(*m_socket,
                      m_request,
                      m_strand.wrap(std::bind(&TcpSession::on_write, shared_from_this(), std::placeholders::_1, std::placeholders::_2)));

    m_request.consume(m_request.size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
