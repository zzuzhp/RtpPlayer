#ifndef ___NETWORKS_CLIENT_H___
#define ___NETWORKS_CLIENT_H___

#include "Dispatcher.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class Client : public Dispatcher
{
public:

    virtual ~Client(){}

    virtual void connect(std::string host, uint16_t port) = 0;

    virtual void connect(std::string host, std::string protocol) = 0;

    template <typename T, typename Y>
    void connect_resolve_handler(T handler, Y * obj)
    {
        m_resolve_handler = std::bind(handler, obj);
    }

protected:

    Client(asio::io_service & io) : Dispatcher(io),
                                    m_resolve_handler(nullptr)
    {

    }

protected:

    net_resolve m_resolve_handler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___NETWORKS_CLIENT_H___
