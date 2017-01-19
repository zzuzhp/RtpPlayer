#ifndef ___NETWORKS_DISPATCHER_H___
#define ___NETWORKS_DISPATCHER_H___

#include "asio/io_service.hpp"
#include "Networks/Defines.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class Dispatcher
{
public:

    virtual ~Dispatcher(){}

    template <typename T, typename Y>
    void connect_error_handler(T handler, Y * obj)
    {
        m_error_handler = std::bind(handler, obj, std::placeholders::_1, std::placeholders::_2);
    }

protected:

    Dispatcher(asio::io_service & io) : m_error_handler(nullptr),
                                        m_service(io),
                                        m_strand(io)
    {

    }

protected:

    bool check_error(const asio::error_code & err, int param, bool evaluate_express = true)
    {
        if (err && evaluate_express)
        {
            if (m_error_handler != nullptr)
            {
                m_error_handler(err.message(), param);
            }

            return false;
        }

        return true;
    }

protected:

    asio::io_service          & m_service;
    asio::io_service::strand    m_strand;

private:

    net_error                   m_error_handler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___NETWORKS_DISPATCHER_H___
