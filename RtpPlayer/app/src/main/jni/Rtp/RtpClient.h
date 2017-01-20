#ifndef ___RTPCLIENT_H___
#define ___RTPCLIENT_H___

#include "Common/common.h"
#include "Networks/UdpServer.h"
#include "pro_thread.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class IRtpPacket;
class RtpClient;
class RtpClientObserver
{
public:

    virtual void on_rtp_packet(RtpClient * client, IRtpPacket * packet) = 0;

    virtual void on_client_opened(RtpClient * client) = 0;

    virtual void on_client_closed(RtpClient * client) = 0;
};

class RtpClient : public CProThreadBase
{
public:

    RtpClient();

    ~RtpClient();

    bool build(RtpClientObserver * observer, uint16_t port);

    void tear();

private:

    void on_network_error(std::string message, size_t bytes);

    void on_network_read(const char * data, size_t bytes);

    void on_network_read_complete(size_t required, size_t transferred);

    void on_network_accept(UdpSessionRef session);

    void read();

    void Svc();

private:

    volatile bool           m_exit;

    RtpClientObserver     * m_observer;

    UdpSessionRef           m_udp_session;

    UdpServerRef            m_server;

    asio::io_service        m_service;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___RTPCLIENT_H___
