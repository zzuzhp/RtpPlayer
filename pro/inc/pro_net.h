#if !defined(____PRO_NET_H____)
#define ____PRO_NET_H____

#include <cstddef>

#if !defined(____PRO_A_H____)
#define ____PRO_A_H____
#define PRO_INT16_VC    short
#define PRO_INT32_VC    int
#define PRO_INT64_VC    __int64
#define PRO_UINT16_VC   unsigned short
#define PRO_UINT32_VC   unsigned int
#define PRO_UINT64_VC   unsigned __int64
#define PRO_CDECL_VC    __cdecl
#define PRO_STDCALL_VC  __stdcall
#define PRO_EXPORT_VC   __declspec(dllexport)
#define PRO_IMPORT_VC   __declspec(dllimport)
#define PRO_INT16_GCC   short
#define PRO_INT32_GCC   int
#define PRO_INT64_GCC   long long
#define PRO_UINT16_GCC  unsigned short
#define PRO_UINT32_GCC  unsigned int
#define PRO_UINT64_GCC  unsigned long long
#define PRO_CDECL_GCC   __attribute__((__cdecl__))
#define PRO_STDCALL_GCC __attribute__((__stdcall__))
#define PRO_EXPORT_GCC  __attribute__((visibility("default")))
#define PRO_IMPORT_GCC
#if defined(_MSC_VER)
#if !defined(PRO_INT16)
#define PRO_INT16   PRO_INT16_VC
#endif
#if !defined(PRO_INT32)
#define PRO_INT32   PRO_INT32_VC
#endif
#if !defined(PRO_INT64)
#define PRO_INT64   PRO_INT64_VC
#endif
#if !defined(PRO_UINT16)
#define PRO_UINT16  PRO_UINT16_VC
#endif
#if !defined(PRO_UINT32)
#define PRO_UINT32  PRO_UINT32_VC
#endif
#if !defined(PRO_UINT64)
#define PRO_UINT64  PRO_UINT64_VC
#endif
#if !defined(PRO_CDECL)
#define PRO_CDECL   PRO_CDECL_VC
#endif
#if !defined(PRO_STDCALL)
#define PRO_STDCALL PRO_STDCALL_VC
#endif
#if !defined(PRO_EXPORT)
#define PRO_EXPORT  PRO_EXPORT_VC
#endif
#if !defined(PRO_IMPORT)
#define PRO_IMPORT  PRO_IMPORT_VC
#endif
#else //// _MSC_VER
#if !defined(PRO_INT16)
#define PRO_INT16   PRO_INT16_GCC
#endif
#if !defined(PRO_INT32)
#define PRO_INT32   PRO_INT32_GCC
#endif
#if !defined(PRO_INT64)
#define PRO_INT64   PRO_INT64_GCC
#endif
#if !defined(PRO_UINT16)
#define PRO_UINT16  PRO_UINT16_GCC
#endif
#if !defined(PRO_UINT32)
#define PRO_UINT32  PRO_UINT32_GCC
#endif
#if !defined(PRO_UINT64)
#define PRO_UINT64  PRO_UINT64_GCC
#endif
#if !defined(PRO_CDECL)
#define PRO_CDECL
#endif
#if !defined(PRO_STDCALL)
#define PRO_STDCALL
#endif
#if !defined(PRO_EXPORT)
#define PRO_EXPORT  PRO_EXPORT_GCC
#endif
#if !defined(PRO_IMPORT)
#define PRO_IMPORT  PRO_IMPORT_GCC
#endif
#endif //// _MSC_VER
#endif //// ____PRO_A_H____

#if !defined(____PRO_EXTERN_C_H____)
#define ____PRO_EXTERN_C_H____
#if defined(__cplusplus)
#define PRO_EXTERN_C1 extern "C" {
#define PRO_EXTERN_C2 }
#else //// __cplusplus
#define PRO_EXTERN_C1
#define PRO_EXTERN_C2
#endif //// __cplusplus
#endif //// ____PRO_EXTERN_C_H____

PRO_EXTERN_C1

/////////////////////////////////////////////////////////////////////////////
////

#if defined(PRO_NET_LIB)
#define PRO_NET_API
#elif defined(PRO_NET_EXPORTS)
#if defined(_MSC_VER)
#define PRO_NET_API //// .def
#else
#define PRO_NET_API PRO_EXPORT
#endif
#else
#define PRO_NET_API PRO_IMPORT
#endif

class IProAcceptor;      //// 接受器
class IProConnector;     //// 连接器
class IProOnTimer;       //// 定时事件处理器
class IProTcpHandshaker; //// tcp握手器

////
//// [[[[ 传输类型
////
typedef unsigned char PRO_TRANS_TYPE;

static const PRO_TRANS_TYPE PRO_TRANS_TCP   = 1;
static const PRO_TRANS_TYPE PRO_TRANS_UDP   = 2;
static const PRO_TRANS_TYPE PRO_TRANS_MCAST = 3;
////
//// ]]]]
////

/////////////////////////////////////////////////////////////////////////////
////

////
//// 反应器
////
//// 这里只暴露了定时器接口,目的是为了在使用反应器的时候,可以方便地
//// 使用其内部的定时器功能,免得还要创建额外的CProTimerFactory对象
////
class IProReactor
{
public:
    
    ////
    //// 创建一个普通定时器
    ////
    //// 返回值为定时器id,0无效
    ////
    virtual unsigned long PRO_STDCALL ScheduleTimer(
        IProOnTimer* onTimer,
        PRO_UINT64   timeSpan, //// 定时周期(ms)
        bool         recurring //// 是否重复
        ) = 0;
    
    ////
    //// 删除一个普通定时器
    ////
    virtual void PRO_STDCALL CancelTimer(unsigned long timerId) = 0;
    
    ////
    //// 创建一个多媒体定时器
    ////
    //// 返回值为定时器id,0无效
    ////
    virtual unsigned long PRO_STDCALL ScheduleMmTimer(
        IProOnTimer* onTimer,
        PRO_UINT64   timeSpan, //// 定时周期(ms)
        bool         recurring //// 是否重复
        ) = 0;
    
    ////
    //// 删除一个多媒体定时器
    ////
    virtual void PRO_STDCALL CancelMmTimer(unsigned long timerId) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 接受器回调目标
////
//// 使用者需要实现该接口
////
class IProAcceptorObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 有连接进入时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnAccept(
        IProAcceptor*  acceptor,
        long           sockId, //// -1无效,接受失败
        const char*    remoteIp,
        unsigned short remotePort
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 连接器回调目标
////
//// 使用者需要实现该接口
////
class IProConnectorObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 连接成功或连接失败时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnConnect(
        IProConnector* connector,
        long           sockId, //// -1无效,连接失败
        const char*    remoteIp,
        unsigned short remotePort
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// tcp握手器回调目标
////
//// 使用者需要实现该接口
////
class IProTcpHandshakerObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 握手数据抵达时,该函数将被回调
    ////
    //// 握手完成后,使用者应该把sockId包装成(IProTransport*),
    //// 或释放sockId对应的套接字资源
    ////
    virtual void PRO_STDCALL OnHandshake(
        IProTcpHandshaker* handshaker,
        long               sockId, //// -1无效,握手失败
        const void*        buf,
        unsigned long      size
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 接收池
////
//// 必须在IProTransportObserver::OnRecv(...)的线程上下文里使用,否则不安全
////
//// 对于tcp,使用环型接收池.
//// 由于tcp是流式工作的,收端动力与发端动力不一定相同,所以,在OnRecv(...)里必须尽可能地
//// 收完全部数据,除非逻辑上数据还不完整;否则,当接收池满了并且底层套接字有数据抵达时,
//// 反应器会一直报告有数据抵达(除非挂起接收能力),导致cpu升高
////
//// 对于udp,使用线性接收池.
//// 为防止EMSGSIZE错误,在OnRecv(...)里必须收完全部数据
////
class IProRecvPool
{
public:
    
    ////
    //// 查询收到的字节数
    ////
    virtual unsigned long PRO_STDCALL PeekDataSize() const = 0;
    
    ////
    //// 复制收到的数据
    ////
    virtual void PRO_STDCALL PeekData(void* buf, unsigned long size) const = 0;
    
    ////
    //// 刷掉已经复制的数据
    ////
    //// 腾出空间,以便接收新的数据
    ////
    virtual void PRO_STDCALL Flush(unsigned long size) = 0;
};

////
//// 传输器
////
class IProTransport
{
public:
    
    ////
    //// 获取传输类型
    ////
    virtual PRO_TRANS_TYPE PRO_STDCALL GetType() const = 0;
    
    ////
    //// 获取底层的套接字id
    ////
    //// 如非必须,最好不要直接操作底层的套接字
    ////
    virtual long PRO_STDCALL GetSockId() const = 0;
    
    ////
    //// 获取套接字的本地ip地址
    ////
    virtual const char* PRO_STDCALL GetLocalIp(char localIp[64]) const = 0;
    
    ////
    //// 获取套接字的本地端口号
    ////
    virtual unsigned short PRO_STDCALL GetLocalPort() const = 0;
    
    ////
    //// 获取套接字的远端ip地址
    ////
    //// 对于tcp,返回连接对端的ip地址;
    //// 对于udp,返回默认的远端ip地址
    ////
    virtual const char* PRO_STDCALL GetRemoteIp(char remoteIp[64]) const = 0;
    
    ////
    //// 获取套接字的远端端口号
    ////
    //// 对于tcp,返回连接对端的端口号;
    //// 对于udp,返回默认的远端端口号
    ////
    virtual unsigned short PRO_STDCALL GetRemotePort() const = 0;
    
    ////
    //// 获取接收池
    ////
    //// 参见IProRecvPool的注释
    ////
    virtual IProRecvPool* PRO_STDCALL GetRecvPool() = 0;
    
    ////
    //// 发送数据
    ////
    //// 对于tcp,数据将放到发送池里,忽略remoteAddr和remoteAddrLen参数;
    //// 对于udp,数据将直接发送,如果remoteAddr或remoteAddrLen参数无效,
    //// 则使用默认的远端地址
    ////
    //// 如果返回false,表示发送池已满,上层负责缓冲数据以及重新发送
    ////
    virtual bool PRO_STDCALL SendData(
        const void*               buf,
        unsigned long             size,
        const struct sockaddr_in* remoteAddr    = NULL, //// for udp
        unsigned long             remoteAddrLen = 0     //// for udp
        ) = 0;
    
    ////
    //// 请求回调一个OnSend事件
    ////
    virtual void PRO_STDCALL RequestOnSend() = 0;
    
    ////
    //// 挂起接收能力
    ////
    virtual void PRO_STDCALL SuspendRecv() = 0;
    
    ////
    //// 恢复接收能力
    ////
    virtual void PRO_STDCALL ResumeRecv() = 0;
    
    ////
    //// 启动心跳事件
    ////
    //// 心跳事件发生时,IProTransportObserver::OnHeartbeat(...)将被回调.
    //// 默认的心跳周期为10秒
    ////
    virtual void PRO_STDCALL StartHeartbeat(
        unsigned long heartbeatInSeconds = 0
        ) = 0;
    
    ////
    //// 停止心跳事件
    ////
    virtual void PRO_STDCALL StopHeartbeat() = 0;
};

////
//// 传输器回调目标
////
//// 使用者需要实现该接口
////
class IProTransportObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 数据抵达套接字的系统缓冲区时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnRecv(
        IProTransport*            trans,
        const struct sockaddr_in* remoteAddr,
        unsigned long             remoteAddrLen
        ) = 0;
    
    ////
    //// 数据被成功送入套接字的系统缓冲区时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnSend(IProTransport* trans) = 0;
    
    ////
    //// 套接字出现错误时,该函数将被回调
    ////
    //// errorCode为系统错误码
    ////
    virtual void PRO_STDCALL OnClose(
        IProTransport* trans,
        long           errorCode
        ) = 0;
    
    ////
    //// 心跳事件发生时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnHeartbeat(IProTransport* trans) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 功能: 初始化网络库
////
//// 参数: 无
////
//// 返回值: 无
////
//// 说明: 无
////
PRO_NET_API
void
PRO_STDCALL
ProInit();

////
//// 功能: 创建一个反应器
////
//// 参数:
//// threadCountWr : 处理发送事件的线程数.客户端推荐2,服务器推荐15
//// threadCountRd : 处理接收事件的线程数.客户端推荐2,服务器推荐15
////
//// 返回值: 反应器对象或空
////
//// 说明: 反应器是主动对象,内部包含线程池.要确保删除动作在锁外执行,否则可能死锁
////
PRO_NET_API
IProReactor*
PRO_STDCALL
ProCreateReactor(unsigned long threadCountWr = 0,
                 unsigned long threadCountRd = 0);

////
//// 功能: 删除一个反应器
////
//// 参数:
//// reactor : 反应器对象
////
//// 返回值: 无
////
//// 说明: 反应器是主动对象,内部包含线程池.要确保删除动作在锁外执行,否则可能死锁
////
PRO_NET_API
void
PRO_STDCALL
ProDeleteReactor(IProReactor* reactor);

////
//// 功能: 创建一个接受器
////
//// 参数:
//// observer        : 回调目标
//// reactor         : 反应器
//// localIp         : 要监听的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort       : 要监听的端口号.如果为0,系统将随机分配一个端口号
//// sockBufSizeRecv : 套接字的系统接收缓冲区字节数.默认(1024 * 56)
//// sockBufSizeSend : 套接字的系统发送缓冲区字节数.默认(1024 * 8)
////
//// 返回值: 接受器对象或空
////
//// 说明: 可以使用ProGetAcceptorPort(...)获取实际的端口号
////
PRO_NET_API
IProAcceptor*
PRO_STDCALL
ProCreateAcceptor(IProAcceptorObserver* observer,
                  IProReactor*          reactor,
                  const char*           localIp         = NULL,
                  unsigned short        localPort       = 0,
                  unsigned long         sockBufSizeRecv = 0,
                  unsigned long         sockBufSizeSend = 0);

////
//// 功能: 获取接受器监听的端口号
////
//// 参数:
//// acceptor : 接受器对象
////
//// 返回值: 端口号
////
//// 说明: 主要用于随机监听的端口号的获取
////
PRO_NET_API
unsigned short
PRO_STDCALL
ProGetAcceptorPort(IProAcceptor* acceptor);

////
//// 功能: 删除一个接受器
////
//// 参数:
//// acceptor : 接受器对象
////
//// 返回值: 无
////
//// 说明: 无
////
PRO_NET_API
void
PRO_STDCALL
ProDeleteAcceptor(IProAcceptor* acceptor);

////
//// 功能: 创建一个连接器
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// remoteIp         : 远端的ip地址或域名
//// remotePort       : 远端的端口号
//// sockBufSizeRecv  : 套接字的系统接收缓冲区字节数.默认(1024 * 56)
//// sockBufSizeSend  : 套接字的系统发送缓冲区字节数.默认(1024 * 8)
//// timeoutInSeconds : 连接超时.默认60秒
//// localBindIp      : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
////
//// 返回值: 连接器对象或空
////
//// 说明: 无
////
PRO_NET_API
IProConnector*
PRO_STDCALL
ProCreateConnector(IProConnectorObserver* observer,
                   IProReactor*           reactor,
                   const char*            remoteIp,
                   unsigned short         remotePort,
                   unsigned long          sockBufSizeRecv  = 0,
                   unsigned long          sockBufSizeSend  = 0,
                   unsigned long          timeoutInSeconds = 0,
                   const char*            localBindIp      = NULL);

////
//// 功能: 删除一个连接器
////
//// 参数:
//// connector : 连接器对象
////
//// 返回值: 无
////
//// 说明: 无
////
PRO_NET_API
void
PRO_STDCALL
ProDeleteConnector(IProConnector* connector);

////
//// 功能: 创建一个tcp握手器
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// sockId           : 套接字id.来自于OnAccept(...)或OnConnect(...)
//// recvDataSize     : 希望接收的字节数
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: tcp握手器对象或空
////
//// 说明: 无
////
PRO_NET_API
IProTcpHandshaker*
PRO_STDCALL
ProCreateTcpHandshaker(IProTcpHandshakerObserver* observer,
                       IProReactor*               reactor,
                       long                       sockId,
                       unsigned long              recvDataSize,
                       unsigned long              timeoutInSeconds = 0);

////
//// 功能: 删除一个tcp握手器
////
//// 参数:
//// handshaker : tcp握手器对象
////
//// 返回值: 无
////
//// 说明: 无
////
PRO_NET_API
void
PRO_STDCALL
ProDeleteTcpHandshaker(IProTcpHandshaker* handshaker);

////
//// 功能: 创建一个tcp传输器
////
//// 参数:
//// observer     : 回调目标
//// reactor      : 反应器
//// sockId       : 套接字id.来自于OnAccept(...)或OnConnect(...)
//// recvPoolSize : 接收池的字节数.默认(1024 * 65)
////
//// 返回值: 传输器对象或空
////
//// 说明: 要防止逻辑悖论发生,即发端不应该发送大于收端接收池尺寸的数据,
////       否则,收端永远得不到希望的数据
////
PRO_NET_API
IProTransport*
PRO_STDCALL
ProCreateTcpTransport(IProTransportObserver* observer,
                      IProReactor*           reactor,
                      long                   sockId,
                      unsigned long          recvPoolSize = 0);

////
//// 功能: 创建一个udp传输器
////
//// 参数:
//// observer        : 回调目标
//// reactor         : 反应器
//// localIp         : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort       : 要绑定的端口号.如果为0,系统将随机分配一个端口号
//// remoteIp        : 默认的远端ip地址或域名
//// remotePort      : 默认的远端端口号
//// sockBufSizeRecv : 套接字的系统接收缓冲区字节数.默认(1024 * 56)
//// sockBufSizeSend : 套接字的系统发送缓冲区字节数.默认(1024 * 56)
////
//// 返回值: 传输器对象或空
////
//// 说明: 可以使用IProTransport::GetLocalPort(...)获取实际的端口号
////
PRO_NET_API
IProTransport*
PRO_STDCALL
ProCreateUdpTransport(IProTransportObserver* observer,
                      IProReactor*           reactor,
                      const char*            localIp           = NULL,
                      unsigned short         localPort         = 0,
                      const char*            defaultRemoteIp   = NULL,
                      unsigned short         defaultRemotePort = 0,
                      unsigned long          sockBufSizeRecv   = 0,
                      unsigned long          sockBufSizeSend   = 0);

////
//// 功能: 创建一个多播传输器
////
//// 参数:
//// observer        : 回调目标
//// reactor         : 反应器
//// mcastIp         : 要绑定的多播地址
//// mcastPort       : 要绑定的多播端口号.如果为0,系统将随机分配一个端口号
//// sockBufSizeRecv : 套接字的系统接收缓冲区字节数.默认(1024 * 56)
//// sockBufSizeSend : 套接字的系统发送缓冲区字节数.默认(1024 * 56)
//// localBindIp     : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
////
//// 返回值: 传输器对象或空
////
//// 说明: 合法的多播地址为(224.0.0.0 ~ 239.255.255.255),
////       推荐的多播地址为(224.0.1.0 ~ 238.255.255.255),
////       RFC-1112(IGMPv1), RFC-2236(IGMPv2), RFC-3376(IGMPv3)
////
////       可以使用IProTransport::GetLocalPort(...)获取实际的端口号
////
PRO_NET_API
IProTransport*
PRO_STDCALL
ProCreateMcastTransport(IProTransportObserver* observer,
                        IProReactor*           reactor,
                        const char*            mcastIp,
                        unsigned short         mcastPort       = 0,
                        unsigned long          sockBufSizeRecv = 0,
                        unsigned long          sockBufSizeSend = 0,
                        const char*            localBindIp     = NULL);

////
//// 功能: 删除一个传输器
////
//// 参数:
//// trans : 传输器对象
////
//// 返回值: 无
////
//// 说明: 无
////
PRO_NET_API
void
PRO_STDCALL
ProDeleteTransport(IProTransport* trans);

////
//// 功能: 打开一个tcp套接字
////
//// 参数:
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort : 要绑定的端口号
////
//// 返回值: 套接字id或-1
////
//// 说明: 主要用于保留rtcp端口
////
PRO_NET_API
long
PRO_STDCALL
ProOpenTcpSockId(const char*    localIp, /* = NULL */
                 unsigned short localPort);

////
//// 功能: 打开一个udp/mcast套接字
////
//// 参数:
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort : 要绑定的端口号
////
//// 返回值: 套接字id或-1
////
//// 说明: 主要用于保留rtcp端口
////
PRO_NET_API
long
PRO_STDCALL
ProOpenUdpSockId(const char*    localIp, /* = NULL */
                 unsigned short localPort);

////
//// 功能: 关闭一个套接字
////
//// 参数:
//// sockId : 套接字id
////
//// 返回值: 无
////
//// 说明: OnAccept(...)或OnConnect(...)会产生套接字,当产生的套接字没有成功包装成
////       (IProTransport*)时,应该使用该函数释放sockId对应的套接字资源
////
PRO_NET_API
void
PRO_STDCALL
ProCloseSockId(long sockId);

/////////////////////////////////////////////////////////////////////////////
////

PRO_EXTERN_C2

#endif //// ____PRO_NET_H____
