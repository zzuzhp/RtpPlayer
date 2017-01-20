#include "Rtp/RtpClient.h"
#include "Rtp/RtpPacket.h"
#include "Networks/UdpServer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

RtpClient::RtpClient() : m_observer(nullptr),
                         m_udp_session(nullptr),
                         m_server(nullptr),
                         m_exit(false)
{

}

RtpClient::~RtpClient()
{
    tear();
}

bool
RtpClient::build(RtpClientObserver * observer, uint16_t port)
{
    m_server = UdpServer::create_instance(m_service);
    if (m_server == nullptr)
    {
        RP_LOG_E("UdpServer create failed.");
        return false;
    }

    m_observer = observer;

    m_server->connect_error_handler(&RtpClient::on_network_error, this);
    m_server->connect_accept_handler(&RtpClient::on_network_accept, this);

    m_server->accept(port);

    Spawn(false);

    return true;
}

void
RtpClient::tear()
{
    m_exit = true;
    m_service.stop();

    Wait();
}

void
RtpClient::on_network_error(std::string message, size_t bytes)
{
    RP_LOG_E("network error: %s.", message.c_str());
}

void
RtpClient::on_network_read(const char * data, size_t bytes)
{
    const char * payload = NULL;
    uint16_t size = 0;
    RTP_HEADER header;

    if (!m_observer)
    {
        return;
    }

    if (!RtpPacket::parse(data, (uint16_t)bytes, header, payload, size))
    {
        RP_LOG_E("RtpPacket parse failed.");
        return;
    }

    IRtpPacket * packet = RtpPacket::create_instance(header, payload, size);
    if (!packet)
    {
        RP_LOG_E("RtpPacket create failed.");
    }
    else
    {
        m_observer->on_rtp_packet(this, packet);
        packet->Release();
    }

    read();
}

void
RtpClient::on_network_read_complete(size_t required, size_t transferred)
{
    RP_LOG_I("network read complete: required: %d, transferred: %d.", required, transferred);
}

void
RtpClient::on_network_accept(UdpSessionRef session)
{
    /* network ready */
    RP_LOG_I("on_network_accept");

    session->connect_read_handler(&RtpClient::on_network_read, this);
    session->connect_read_complete_handler(&RtpClient::on_network_read_complete, this);

    if (m_observer)
    {
        m_observer->on_client_opened(this);
    }

    m_udp_session = session;

    read();
}

void
RtpClient::read()
{
    /* read from the web */
    if (!m_exit && m_udp_session)
    {
        m_udp_session->read();
    }
}

void
RtpClient::Svc()
{
    RP_LOG_E("RtpClient::Svc() in");
    /* run the service */
    asio::io_service::work work(m_service);
    m_service.run();

    RP_LOG_E("RtpClient::Svc() out");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////
