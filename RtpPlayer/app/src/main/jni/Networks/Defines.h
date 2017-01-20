#ifndef ___DEFINES_H___
#define ___DEFINES_H___

#include <string>
#include <functional>
#include "asio/asio.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////

class UdpSession;
class UdpClient;
class UdpServer;
typedef std::shared_ptr<UdpClient>                  UdpClientRef;
typedef std::shared_ptr<UdpServer>                  UdpServerRef;
typedef std::shared_ptr<asio::ip::udp::resolver>    UdpResolverRef;
typedef std::shared_ptr<UdpSession>                 UdpSessionRef;
typedef std::shared_ptr<asio::ip::udp::socket>      UdpSocketRef;

/*!
 * Network callbacks
 */

typedef std::function<void (std::string, size_t)>   net_error;
typedef std::function<void (const char *, size_t)>  net_read;
typedef std::function<void (size_t, size_t)>        net_read_complete;
typedef std::function<void (size_t)>                net_write;
typedef std::function<void ()>                      net_resolve;
typedef std::function<void (UdpSessionRef)>         net_connect;
typedef std::function<void (UdpSessionRef)>         net_accept;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___DEFINES_H___
