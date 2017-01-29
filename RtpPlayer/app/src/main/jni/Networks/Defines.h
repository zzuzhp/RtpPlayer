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
class TcpSession;
class TcpClient;
class TcpServer;
typedef std::shared_ptr<UdpClient>                  UdpClientRef;
typedef std::shared_ptr<TcpClient>                  TcpClientRef;
typedef std::shared_ptr<UdpServer>                  UdpServerRef;
typedef std::shared_ptr<TcpServer>                  TcpServerRef;
typedef std::shared_ptr<asio::ip::udp::resolver>    UdpResolverRef;
typedef std::shared_ptr<asio::ip::tcp::resolver>    TcpResolverRef;
typedef std::shared_ptr<UdpSession>                 UdpSessionRef;
typedef std::shared_ptr<TcpSession>                 TcpSessionRef;
typedef std::shared_ptr<asio::ip::tcp::acceptor>    TcpAcceptorRef;
typedef std::shared_ptr<asio::ip::udp::socket>      UdpSocketRef;
typedef std::shared_ptr<asio::ip::tcp::socket>      TcpSocketRef;
typedef std::shared_ptr<asio::io_service>           IoServiceRef;

/*!
 * Network callbacks
 */

typedef std::function<void (std::string, size_t)>   net_error;
typedef std::function<void (const char *, size_t)>  net_read;
typedef std::function<void (size_t, size_t)>        net_read_complete;
typedef std::function<void (size_t)>                net_write;
typedef std::function<void ()>                      net_resolve;
typedef std::function<void (UdpSessionRef)>         net_connect;
typedef std::function<void (TcpSessionRef)>         net_tcp_connect;
typedef std::function<void (UdpSessionRef)>         net_accept;
typedef std::function<void (TcpSessionRef)>         net_tcp_accept;
typedef std::function<void ()>                      net_close;
typedef std::function<void ()>                      net_cancel;

////////////////////////////////////////////////////////////////////////////////////////////////////
////

#endif ///< ___DEFINES_H___
