#ifndef ___TCPCLIENT_H___
#define ___TCPCLIENT_H___

#include "Client.h"
#include "TcpSession.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class TcpClient : public Client,
                  public std::enable_shared_from_this<TcpClient>
{
public:

    static TcpClientRef create_instance(asio::io_service & io);

    virtual ~TcpClient();

    void connect(const std::string host, uint16_t port);
    void connect(const std::string host, const std::string protocol);

    template <typename T, typename Y>
    inline void connect_connect_handler(T handle, Y * obj)
    {
        m_connect_handler = std::bind(handle, obj, std::placeholders::_1);
    }

    const TcpResolverRef & resolver() const { return m_resolver; }

protected:

    TcpClient(asio::io_service & io);

    virtual void on_connect(TcpSessionRef session, const asio::error_code & err);
    virtual void on_resolve(const asio::error_code & err, asio::ip::tcp::resolver::iterator iter);

protected:

    TcpResolverRef      m_resolver;
    net_tcp_connect     m_connect_handler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___TCPCLIENT_H___
