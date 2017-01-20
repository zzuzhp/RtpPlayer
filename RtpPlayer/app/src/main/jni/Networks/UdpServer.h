#ifndef ___UDPSERVER_H___
#define ___UDPSERVER_H___

#include "Server.h"
#include "UdpSession.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class UdpServer : public Server,
                  public std::enable_shared_from_this<UdpServer>
{
public:

    static UdpServerRef create_instance(asio::io_service & io);

    virtual ~UdpServer();

    void accept(uint16_t port);

    template <typename T, typename Y>
    void connect_accept_handler(T handler, Y * obj)
    {
        m_accept_handler = std::bind(handler, obj, std::placeholders::_1);
    }

protected:

    UdpServer(asio::io_service & io);

protected:

    net_accept      m_accept_handler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___UDPSERVER_H___
