#if !defined(____RTP_FOUNDATION_H____)
#define ____RTP_FOUNDATION_H____

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

#if defined(RTP_FOUNDATION_LIB)
#define RTP_FOUNDATION_API
#elif defined(RTP_FOUNDATION_EXPORTS)
#if defined(_MSC_VER)
#define RTP_FOUNDATION_API //// .def
#else
#define RTP_FOUNDATION_API PRO_EXPORT
#endif
#else
#define RTP_FOUNDATION_API PRO_IMPORT
#endif

#if !defined(____RTP_FRAMEWORK_H____)
#define RTP_MM_TYPE      unsigned char
#define RTP_SESSION_TYPE unsigned char
#endif

class  IProReactor;
class  IRtpAcceptorObserver;
class  IRtpBucket;
class  IRtpPacket;
class  IRtpSession;
class  IRtpSessionObserver;
class  IRtpTunnelClient;
class  IRtpTunnelServer;
struct RTP_SESSION_INFO;

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_UDPCLIENT
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    bool                 antiNat;
    char                 localIp[64];      /* = "" */
    unsigned short       localPort;        /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_UDPSERVER
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    bool                 antiNat;
    char                 localIp[64];      /* = "" */
    unsigned short       localPort;        /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_TCPCLIENT
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 remoteIp[64];
    unsigned short       remotePort;
    char                 localIp[64];      /* = "" */
    unsigned long        timeoutInSeconds; /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_TCPSERVER
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 localIp[64];      /* = "" */
    unsigned short       localPort;        /* = 0 */
    unsigned long        timeoutInSeconds; /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_UDPCLIENT_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 remoteIp[64];
    unsigned short       remotePort;
    char                 localIp[64];      /* = "" */
    unsigned long        timeoutInSeconds; /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_UDPSERVER_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 localIp[64];      /* = "" */
    unsigned short       localPort;        /* = 0 */
    unsigned long        timeoutInSeconds; /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_TCPCLIENT_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 remoteIp[64];
    unsigned short       remotePort;
    char                 localIp[64];      /* = "" */
    char                 password[256];    /* = "" */
    unsigned long        timeoutInSeconds; /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_TCPSERVER_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    long                 sockId;
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_SSLCLIENT_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 remoteIp[64];
    unsigned short       remotePort;
    char                 localIp[64];      /* = "" */
    char                 password[256];    /* = "" */
    unsigned long        timeoutInSeconds; /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_SSLSERVER_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    long                 sockId;
    bool                 xorOnly;
    unsigned char        sessionKeyBytes;  /* = 0 */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_MCASTCLIENT
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 mcastIp[64];
    unsigned short       mcastPort;        /* = 0 */
    char                 localIp[64];      /* = "" */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_MCASTSERVER
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 mcastIp[64];
    unsigned short       mcastPort;        /* = 0 */
    char                 localIp[64];      /* = "" */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_MCASTCLIENT_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 mcastIp[64];
    unsigned short       mcastPort;        /* = 0 */
    char                 localIp[64];      /* = "" */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
//// 如果bucket为NULL,系统将自动分配一个rtp桶
////
struct RTP_INIT_MCASTSERVER_EX
{
    IRtpSessionObserver* observer;
    IProReactor*         reactor;
    char                 mcastIp[64];
    unsigned short       mcastPort;        /* = 0 */
    char                 localIp[64];      /* = "" */
    IRtpBucket*          bucket;           /* = NULL */
};

////
//// 会话初始化参数
////
struct RTP_INIT_COMMON
{
    IRtpSessionObserver* observer_r;       //// [只读]
    IProReactor*         reactor_r;        //// [只读]
};

////
//// 会话初始化参数集合
////
struct RTP_INIT_ARGS
{
    union
    {
        RTP_INIT_UDPCLIENT      udpclient;
        RTP_INIT_UDPSERVER      udpserver;
        RTP_INIT_TCPCLIENT      tcpclient;
        RTP_INIT_TCPSERVER      tcpserver;
        RTP_INIT_UDPCLIENT_EX   udpclientEx;
        RTP_INIT_UDPSERVER_EX   udpserverEx;
        RTP_INIT_TCPCLIENT_EX   tcpclientEx;
        RTP_INIT_TCPSERVER_EX   tcpserverEx;
        RTP_INIT_SSLCLIENT_EX   sslclientEx;
        RTP_INIT_SSLSERVER_EX   sslserverEx;
        RTP_INIT_MCASTCLIENT    mcastclient;
        RTP_INIT_MCASTSERVER    mcastserver;
        RTP_INIT_MCASTCLIENT_EX mcastclientEx;
        RTP_INIT_MCASTSERVER_EX mcastserverEx;
        RTP_INIT_COMMON         comm;
    };
};

#pragma pack(push, 1) //// [[[[----------------------------------------------
#pragma pack(pop)     //// ]]]]

////
//// 消息用户
////
//// classId : 该字段用于标识用户类别,以便于应用程序管理.
////           0无效,1服务器节点,2~c客户端节点
////
//// userId  : 该字段用于标识用户id.
////           0动态分配,动态分配范围为(2000000001 ~ u);否则静态分配,
////           静态分配范围为(1 ~ 2000000000)
////
//// 说明    : classId-userId 之 1-1 保留,内部使用
////
struct RTP_MSG_USER
{
    RTP_MSG_USER()
    {
        classId = 0;
        userId  = 0;
    }
    
    bool operator<(const RTP_MSG_USER& user) const
    {
        if (classId < user.classId)
        {
            return (true);
        }
        
        if (classId > user.classId)
        {
            return (false);
        }
        
        if (userId < user.userId)
        {
            return (true);
        }
        
        return (false);
    }
    
    PRO_UINT32 classId;
    PRO_UINT32 userId;
};

#pragma pack(push, 1) //// [[[[
#pragma pack(pop)     //// ]]]]----------------------------------------------

/////////////////////////////////////////////////////////////////////////////
////

////
//// rtp桶
////
//// 该类使用了strategy模式.不同的子类将实现不同的流控策略
////
class IRtpBucket
{
public:
    
    virtual void PRO_STDCALL Destroy() = 0; //// !!!
    
    virtual unsigned long PRO_STDCALL GetTotalBytes() const = 0;
    
    virtual IRtpPacket* PRO_STDCALL GetFront() = 0;
    
    virtual bool PRO_STDCALL PushBackAddRef(IRtpPacket* packet) = 0;
    
    virtual void PRO_STDCALL PopFrontRelease() = 0;
    
    virtual void PRO_STDCALL Reset() = 0;
    
    virtual void PRO_STDCALL SetRedlineBytes(unsigned long redlineBytes) = 0;
    
    virtual void PRO_STDCALL SetRedlineFrames(unsigned long redlineFrames) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 消息客户端
////
class IRtpMsgClient
{
public:
    
    ////
    //// 获取用户信息
    ////
    virtual const RTP_MSG_USER PRO_STDCALL GetUserInfo() const = 0;
    
    ////
    //// 发送消息
    ////
    //// 系统内部有消息发送队列
    ////
    virtual bool PRO_STDCALL SendMsg(
        const void*         buf,         //// 消息内容
        PRO_UINT16          size,        //// 最多(1024 * 60)字节
        const RTP_MSG_USER* dstUsers,    //// 消息接收者
        unsigned char       dstUserCount //// 最多255个目标
        ) = 0;
};

////
//// 消息客户端回调目标
////
//// 使用者需要实现该接口
////
class IRtpMsgClientObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 状态就绪时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnOkMsg(
        IRtpMsgClient* msgClient,
        const char*    myPublicIp
        ) = 0;
    
    ////
    //// 消息抵达时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnRecvMsg(
        IRtpMsgClient*      msgClient,
        const void*         buf,
        PRO_UINT16          size,
        const RTP_MSG_USER* srcUser
        ) = 0;
    
    ////
    //// 出现错误时,该函数将被回调
    ////
    //// errorCode为系统错误码
    ////
    virtual void PRO_STDCALL OnCloseMsg(
        IRtpMsgClient* msgClient,
        long           errorCode
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 消息服务器
////
class IRtpMsgServer
{
public:
    
    ////
    //// 获取用户数
    ////
    virtual void PRO_STDCALL GetUserCount(
        unsigned long* pendingUserCount,
        unsigned long* baseUserCount,
        unsigned long* subUserCount
        ) const = 0;
};

////
//// 消息服务器回调目标
////
//// 使用者需要实现该接口
////
class IRtpMsgServerObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 用户请求登录时,该函数将被回调
    ////
    //// 返回值表示是否允许该用户登录
    ////
    virtual bool PRO_STDCALL OnCheckUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* userInfo,
        PRO_UINT32          userToken,
        const char          passwordHash[20],
        const char*         userPublicIp
        ) = 0;
    
    ////
    //// 用户状态就绪时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnOkUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* userInfo,
        const char*         userPublicIp
        ) = 0;
    
    ////
    //// 用户出现错误时,该函数将被回调
    ////
    //// errorCode为系统错误码
    ////
    virtual void PRO_STDCALL OnCloseUser(
        IRtpMsgServer*      msgServer,
        const RTP_MSG_USER* userInfo,
        long                errorCode
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 消息c2s
////
class IRtpMsgC2s
{
public:
    
    ////
    //// 获取c2s信息
    ////
    virtual const RTP_MSG_USER PRO_STDCALL GetC2sInfo() const = 0;
    
    ////
    //// 获取用户数
    ////
    virtual void PRO_STDCALL GetUserCount(
        unsigned long* pendingUserCount,
        unsigned long* userCount
        ) const = 0;
};

////
//// 消息c2s回调目标
////
//// 使用者需要实现该接口
////
class IRtpMsgC2sObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// c2s状态就绪时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnOkC2s(
        IRtpMsgC2s* msgC2s,
        const char* c2sPublicIp
        ) = 0;
    
    ////
    //// c2s出现错误时,该函数将被回调
    ////
    //// errorCode为系统错误码
    ////
    virtual void PRO_STDCALL OnCloseC2s(
        IRtpMsgC2s* msgC2s,
        long        errorCode
        ) = 0;
    
    ////
    //// 用户状态就绪时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnOkUser(
        IRtpMsgC2s*         msgC2s,
        const RTP_MSG_USER* userInfo,
        const char*         userPublicIp
        ) = 0;
    
    ////
    //// 用户出现错误时,该函数将被回调
    ////
    //// errorCode为系统错误码
    ////
    virtual void PRO_STDCALL OnCloseUser(
        IRtpMsgC2s*         msgC2s,
        const RTP_MSG_USER* userInfo,
        long                errorCode
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// 功能: 初始化rtp库
////
//// 参数: 无
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FOUNDATION_API
void
PRO_STDCALL
InitRtpFoundation();

////
//// 功能: 创建一个会话包装
////
//// 参数:
//// sessionType : 会话类型
//// initArgs    : 会话初始化参数集合
//// localInfo   : 会话信息
////
//// 返回值: 会话包装对象或空
////
//// 说明: 会话包装对象包含流控策略
////
RTP_FOUNDATION_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionWrapper(RTP_SESSION_TYPE        sessionType,
                        const RTP_INIT_ARGS*    initArgs,
                        const RTP_SESSION_INFO* localInfo);

////
//// 功能: 删除一个会话包装
////
//// 参数:
//// sessionWrapper : 会话包装对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FOUNDATION_API
void
PRO_STDCALL
DeleteRtpSessionWrapper(IRtpSession* sessionWrapper);

////
//// 功能: 创建一个tunnel客户端
////
//// 参数:
//// observer   : 回调目标
//// reactor    : 反应器
//// tunnelPort : tunnel服务器的端口号
//// mmType     : 媒体类型
////
//// 返回值: tunnel客户端对象或空
////
//// 说明: tunnel服务器可以将不同媒体类型的连接请求派发给相应的tunnel客户端,
////       并允许跨越进程边界(目前仅限于Windows)
////
RTP_FOUNDATION_API
IRtpTunnelClient*
PRO_STDCALL
CreateRtpTunnelClient(IRtpAcceptorObserver* observer,
                      IProReactor*          reactor,
                      unsigned short        tunnelPort,
                      RTP_MM_TYPE           mmType);

////
//// 功能: 删除一个tunnel客户端
////
//// 参数:
//// tunnelClient : tunnel客户端对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FOUNDATION_API
void
PRO_STDCALL
DeleteRtpTunnelClient(IRtpTunnelClient* tunnelClient);

////
//// 功能: 创建一个tunnel服务器
////
//// 参数:
//// reactor          : 反应器
//// tunnelPort       : tunnel服务器的端口号
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: tunnel服务器对象或空
////
//// 说明: tunnel服务器可以将不同媒体类型的连接请求派发给相应的tunnel客户端,
////       并允许跨越进程边界(目前仅限于Windows)
////
RTP_FOUNDATION_API
IRtpTunnelServer*
PRO_STDCALL
CreateRtpTunnelServer(IProReactor*   reactor,
                      unsigned short tunnelPort,
                      unsigned long  timeoutInSeconds = 0);

////
//// 功能: 删除一个tunnel服务器
////
//// 参数:
//// tunnelServer : tunnel服务器对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FOUNDATION_API
void
PRO_STDCALL
DeleteRtpTunnelServer(IRtpTunnelServer* tunnelServer);

////
//// 功能: 创建一个消息客户端
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// remoteIp         : 远端的ip地址或域名
//// remotePort       : 远端的端口号
//// encryptMsg       : 是否加密
//// userInfo         : 用户信息.
////                    如果userId为0,服务器将动态分配一个userId,
////                    动态分配范围为(2000000001 ~ u);否则静态分配,
////                    静态分配范围为(1 ~ 2000000000)
//// password         : 用户口令
//// localIp          : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: 消息客户端对象或空
////
//// 说明: 无
////
RTP_FOUNDATION_API
IRtpMsgClient*
PRO_STDCALL
CreateRtpMsgClient(IRtpMsgClientObserver* observer,
                   IProReactor*           reactor,
                   const char*            remoteIp,
                   unsigned short         remotePort,
                   bool                   encryptMsg,
                   const RTP_MSG_USER*    userInfo,
                   const char*            password         = NULL,
                   const char*            localIp          = NULL,
                   unsigned long          timeoutInSeconds = 0);

////
//// 功能: 删除一个消息客户端
////
//// 参数:
//// msgClient : 消息客户端对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FOUNDATION_API
void
PRO_STDCALL
DeleteRtpMsgClient(IRtpMsgClient* msgClient);

////
//// 功能: 创建一个消息服务器
////
//// 参数:
//// observer        : 回调目标
//// reactor         : 反应器
//// tunnelPort      : tunnel服务器的端口号
//// xorOnly         : 客户端希望加密时,是否仅使用异或运算
//// sessionKeyBytes : 客户端希望加密时,会话密钥的字节数(16/32/64).默认16字节(128位)
////
//// 返回值: 消息服务器对象或空
////
//// 说明: 无
////
RTP_FOUNDATION_API
IRtpMsgServer*
PRO_STDCALL
CreateRtpMsgServer(IRtpMsgServerObserver* observer,
                   IProReactor*           reactor,
                   unsigned short         tunnelPort,
                   bool                   xorOnly,
                   unsigned char          sessionKeyBytes = 0);

////
//// 功能: 删除一个消息服务器
////
//// 参数:
//// msgServer : 消息服务器对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FOUNDATION_API
void
PRO_STDCALL
DeleteRtpMsgServer(IRtpMsgServer* msgServer);

////
//// 功能: 创建一个消息c2s
////
//// 参数:
//// observer               : 回调目标
//// reactor                : 反应器
//// uplinkIp               : 级联时远端的ip地址或域名
//// uplinkPort             : 级联时远端的端口号
//// uplinkEncryptMsg       : 级联时是否加密
//// uplinkUserInfo         : 级联的用户信息.
////                          如果userId为0,服务器将动态分配一个userId,
////                          动态分配范围为(2000000001 ~ u);否则静态分配,
////                          静态分配范围为(1 ~ 2000000000)
//// uplinkPassword         : 级联的用户口令
//// uplinkLocalIp          : 级联时要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// uplinkTimeoutInSeconds : 级联时的握手超时.默认60秒
//// tunnelPort             : 近端的tunnel服务器的端口号
//// xorOnly                : 近端的客户端希望加密时,是否仅使用异或运算
//// sessionKeyBytes        : 近端的客户端希望加密时,会话密钥的字节数(16/32/64).默认16字节(128位)
//// timeoutInSeconds       : 近端的握手超时.默认60秒
////
//// 返回值: 消息c2s对象或空
////
//// 说明: 无
////
RTP_FOUNDATION_API
IRtpMsgC2s*
PRO_STDCALL
CreateRtpMsgC2s(IRtpMsgC2sObserver* observer,
                IProReactor*        reactor,
                const char*         uplinkIp,
                unsigned short      uplinkPort,
                bool                uplinkEncryptMsg,
                const RTP_MSG_USER* uplinkUserInfo,
                const char*         uplinkPassword,
                const char*         uplinkLocalIp,          /* = NULL */
                unsigned long       uplinkTimeoutInSeconds, /* = 0 */
                unsigned short      tunnelPort,
                bool                xorOnly,
                unsigned char       sessionKeyBytes,        /* = 0 */
                unsigned long       timeoutInSeconds);      /* = 0 */

////
//// 功能: 删除一个消息c2s
////
//// 参数:
//// msgC2s : 消息c2s对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FOUNDATION_API
void
PRO_STDCALL
DeleteRtpMsgC2s(IRtpMsgC2s* msgC2s);

////
//// 功能: 创建一个内部实现的基础类型的rtp桶
////
//// 参数: 无
////
//// 返回值: rtp桶对象或空
////
//// 说明: rtp桶的线程安全性由调用者保证
////
RTP_FOUNDATION_API
IRtpBucket*
PRO_STDCALL
CreateRtpBaseBucket();

////
//// 功能: 创建一个内部实现的音频类型的rtp桶
////
//// 参数: 无
////
//// 返回值: rtp桶对象或空
////
//// 说明: rtp桶的线程安全性由调用者保证
////
RTP_FOUNDATION_API
IRtpBucket*
PRO_STDCALL
CreateRtpAudioBucket();

////
//// 功能: 创建一个内部实现的视频类型的rtp桶
////
//// 参数: 无
////
//// 返回值: rtp桶对象或空
////
//// 说明: rtp桶的线程安全性由调用者保证
////
RTP_FOUNDATION_API
IRtpBucket*
PRO_STDCALL
CreateRtpVideoBucket();

/////////////////////////////////////////////////////////////////////////////
////

#if !defined(____RTP_FRAMEWORK_H____)
#undef RTP_MM_TYPE
#undef RTP_SESSION_TYPE
#endif

PRO_EXTERN_C2

#endif //// ____RTP_FOUNDATION_H____
