#ifndef ___TCPSESSION_H___
#define ___TCPSESSION_H___

#include "Session.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class TcpSession : public Session,
                   public std::enable_shared_from_this<TcpSession>
{
    friend class TcpClient;
    friend class TcpServer;
public:

    static TcpSessionRef create_instance(asio::io_service & io);

    ~TcpSession();

    void    close();

    virtual void read();
    virtual void read(size_t size);
    virtual void write(const char * data, size_t size);

    const TcpSocketRef & socket() const { return m_socket; }

    template <typename T, typename Y>
    inline void connect_close_event(T handler, Y * obj)
    {
        m_close = std::bind(handler, obj);
    }

protected:

    TcpSession(asio::io_service & io);

    virtual void on_close(const asio::error_code & err);

protected:

    TcpSocketRef    m_socket;

    net_close       m_close;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___TCPSESSION_H___
