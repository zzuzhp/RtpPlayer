#ifndef ___NETWORKS_UDPSESSION_H___
#define ___NETWORKS_UDPSESSION_H___

#include "Session.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class UdpSession : public Session,
                   public std::enable_shared_from_this<UdpSession>
{
    friend class UdpClient;
    friend class UdpServer;
public:

    static UdpSessionRef create_instance(asio::io_service & io);

    ~UdpSession();

    virtual void read();
    virtual void read(size_t size);
    virtual void write(const char * data, size_t size);

    const asio::ip::udp::endpoint   & local_endpoint() const { return m_local_endpoint; }
    const asio::ip::udp::endpoint   & remote_endpoint() const { return m_remote_endpoint; }
    const UdpSocketRef & socket() const { return m_socket; }

protected:

    UdpSession(asio::io_service & io);

protected:

    asio::ip::udp::endpoint         m_local_endpoint;
    asio::ip::udp::endpoint         m_remote_endpoint;
    UdpSocketRef                    m_socket;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___NETWORKS_UDPSESSION_H___
