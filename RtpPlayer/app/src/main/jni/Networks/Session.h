#ifndef ___NETWORKS_SESSION_H___
#define ___NETWORKS_SESSION_H___

#include "Dispatcher.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class Session : public Dispatcher
{
public:

    virtual ~Session();

    virtual void read() = 0;

    virtual void write(const char * data, size_t size) = 0;

    template <typename T, typename Y>
    void connect_read_handler(T handler, Y * obj)
    {
        m_read_handler = std::bind(handler, obj, std::placeholders::_1, std::placeholders::_2);
    }

    template <typename T, typename Y>
    void connect_read_complete_handler(T handler, Y * obj)
    {
        m_read_complete_handler = std::bind(handler, obj, std::placeholders::_1, std::placeholders::_2);
    }

    template <typename T, typename Y>
    void connect_write_handler(T handler, Y * obj)
    {
        m_write_handler = std::bind(handler, obj, std::placeholders::_1);
    }

protected:

    Session(asio::io_service & io);

    virtual void on_read(const asio::error_code & err, size_t bytes_transferred);

    virtual void on_write(const asio::error_code & err, size_t bytes_transferred);

protected:

    size_t                                      m_buffer_size;
    asio::streambuf                             m_request;
    asio::streambuf                             m_response;

    net_read                                    m_read_handler;
    net_read_complete                           m_read_complete_handler;
    net_write                                   m_write_handler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___NETWORKS_SESSION_H___
