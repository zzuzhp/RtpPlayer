#ifndef ___TCPSERVER_H___
#define ___TCPSERVER_H___

#include "Server.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class TcpServer : public Server,
                  public std::enable_shared_from_this<TcpServer>
{
public:

    static TcpServerRef create_instance(asio::io_service & io);

    ~TcpServer();

    virtual void accept(uint16_t port);
    void    cancel();

    const TcpAcceptorRef & acceptor() const { return m_acceptor; }

    template <typename T, typename Y>
    inline void connect_accept_handler(T handler, Y * obj)
    {
        m_accept_handler = std::bind(handler, obj, std::placeholders::_1);
    }

    template <typename T, typename Y>
    inline void connect_cancel_handler(T handler, Y * obj)
    {
        m_cancel_handler = std::bind(handler, obj);
    }

protected:

    TcpServer(asio::io_service & io);

    void listen();

    void on_accept(TcpSessionRef session, const asio::error_code & err);

protected:

    TcpAcceptorRef      m_acceptor;

    net_tcp_accept      m_accept_handler;
    net_cancel          m_cancel_handler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___TCPSERVER_H___
