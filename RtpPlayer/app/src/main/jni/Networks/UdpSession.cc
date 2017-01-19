#include "UdpSession.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

UdpSessionRef
UdpSession::create_instance(asio::io_service & io)
{
    return UdpSessionRef(new UdpSession(io))->shared_from_this();
}

UdpSession::UdpSession(asio::io_service & io) : Session(io)
{
    m_socket = UdpSocketRef(new asio::ip::udp::socket(io));
}

UdpSession::~UdpSession()
{

}

void
UdpSession::read()
{
    read(64 * 1024); ///< max udp packet size
}

void
UdpSession::read(size_t size)
{
    m_buffer_size = size;
    m_socket->async_receive_from(m_response.prepare(size),
                                 m_remote_endpoint,
                                 m_strand.wrap(std::bind(&UdpSession::on_read,
                                                         shared_from_this(),
                                                         std::placeholders::_1,
                                                         std::placeholders::_2)));
}

void
UdpSession::write(const char * data, size_t size)
{
    std::ostream stream(&m_request);
    stream.write(data, size);

    m_socket->async_send(m_request.data(), m_strand.wrap(std::bind(&UdpSession::on_write,
                                                                   shared_from_this(),
                                                                   std::placeholders::_1,
                                                                   std::placeholders::_2)));

    m_socket->set_option(asio::socket_base::broadcast(true));
    m_local_endpoint = m_socket->local_endpoint();

    m_request.consume(m_request.size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
