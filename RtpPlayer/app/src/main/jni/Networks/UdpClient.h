#ifndef ___NETWORKS_UDPCLIENT_H___
#define ___NETWORKS_UDPCLIENT_H___

#include "Client.h"
#include "UdpSession.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class UdpClient : public Client,
                  public std::enable_shared_from_this<UdpClient>
{
public:

    static UdpClientRef create_instance(asio::io_service & io);

    virtual ~UdpClient();

    void connect(const std::string host, uint16_t port);
    void connect(const std::string host, const std::string protocol);

    template <typename T, typename Y>
    inline void connect_connect_handler(T handler, Y * obj)
    {
        m_connect_handler = std::bind(handler, obj, std::placeholders::_1);
    }

    UdpResolverRef resolver() const { return m_resolver; }

protected:

    UdpClient(asio::io_service & io);

    virtual void on_connect(UdpSessionRef session, const asio::error_code & err);
    virtual void on_resolve(const asio::error_code & err, asio::ip::udp::resolver::iterator itr);

protected:

    UdpResolverRef  m_resolver;
    net_connect     m_connect_handler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___NETWORKS_UDPCLIENT_H___
