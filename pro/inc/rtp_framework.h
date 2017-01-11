#if !defined(____RTP_FRAMEWORK_H____)
#define ____RTP_FRAMEWORK_H____

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

////
//// RFC-1889/1890, RFC-3550/3551, RFC-4571
////

/////////////////////////////////////////////////////////////////////////////
////

#if defined(RTP_FRAMEWORK_LIB)
#define RTP_FRAMEWORK_API
#elif defined(RTP_FRAMEWORK_EXPORTS)
#if defined(_MSC_VER)
#define RTP_FRAMEWORK_API //// .def
#else
#define RTP_FRAMEWORK_API PRO_EXPORT
#endif
#else
#define RTP_FRAMEWORK_API PRO_IMPORT
#endif

class IProReactor;  //// 反应器.参见pro_net.h
class IRtpAcceptor; //// rtp接受器

////
//// [[[[ rtp媒体类型.0~127保留,128~255自定义
////
typedef unsigned char RTP_MM_TYPE;

static const RTP_MM_TYPE RTP_MMT_IPC     = 0; //// 进程间通信.[内部使用]
static const RTP_MM_TYPE RTP_MMT_MSG     = 1; //// 消息
static const RTP_MM_TYPE RTP_MMT_CTRL    = 2; //// 控制
static const RTP_MM_TYPE RTP_MMT_AUDIO   = 3; //// 音频
static const RTP_MM_TYPE RTP_MMT_VIDEO   = 4; //// 视频
static const RTP_MM_TYPE RTP_MMT_VIDEOII = 5; //// 视频II
////
//// ]]]]
////

////
//// [[[[ rtp会话类型
////
typedef unsigned char RTP_SESSION_TYPE;

static const RTP_SESSION_TYPE RTP_ST_UDPCLIENT      = 1;  //// udp-标准rtp协议客户端
static const RTP_SESSION_TYPE RTP_ST_UDPSERVER      = 2;  //// udp-标准rtp协议服务端
static const RTP_SESSION_TYPE RTP_ST_TCPCLIENT      = 3;  //// tcp-标准rtp协议客户端
static const RTP_SESSION_TYPE RTP_ST_TCPSERVER      = 4;  //// tcp-标准rtp协议服务端
static const RTP_SESSION_TYPE RTP_ST_UDPCLIENT_EX   = 5;  //// udp-扩展协议客户端
static const RTP_SESSION_TYPE RTP_ST_UDPSERVER_EX   = 6;  //// udp-扩展协议服务端
static const RTP_SESSION_TYPE RTP_ST_TCPCLIENT_EX   = 7;  //// tcp-扩展协议客户端
static const RTP_SESSION_TYPE RTP_ST_TCPSERVER_EX   = 8;  //// tcp-扩展协议服务端
static const RTP_SESSION_TYPE RTP_ST_SSLCLIENT_EX   = 9;  //// ssl-扩展协议客户端
static const RTP_SESSION_TYPE RTP_ST_SSLSERVER_EX   = 10; //// ssl-扩展协议服务端
static const RTP_SESSION_TYPE RTP_ST_MCASTCLIENT    = 11; //// mcast-标准rtp协议客户端
static const RTP_SESSION_TYPE RTP_ST_MCASTSERVER    = 12; //// mcast-标准rtp协议服务端
static const RTP_SESSION_TYPE RTP_ST_MCASTCLIENT_EX = 13; //// mcast-扩展协议客户端
static const RTP_SESSION_TYPE RTP_ST_MCASTSERVER_EX = 14; //// mcast-扩展协议服务端
////
//// ]]]]
////

////
//// rtp会话信息
////
#pragma pack(push, 1) //// [[[[----------------------------------------------
#pragma pack(pop)     //// ]]]]
struct RTP_SESSION_INFO
{
    PRO_UINT32       roomId;             //// 会议室id
    PRO_UINT32       localMmId;          //// 本地节点id
    PRO_UINT32       remoteMmId;         //// 远端节点id
    
    struct
    {
        bool         enable;             //// 是否允许输入方向的媒体流
        char         __padding__[3];
        PRO_UINT32   srcMmId;            //// 输入方向的媒体流的源节点id.如果为0,不限制id
    }                in;
    
    struct
    {
        bool         enable;             //// 是否允许输出方向的媒体流
        char         __padding__[3];
        PRO_UINT32   srcMmId;            //// 输出方向的媒体流的源节点id.如果为0,不限制id
    }                out;
    
    RTP_MM_TYPE      mmType;             //// 媒体类型
    RTP_SESSION_TYPE sessionType_r;      //// 会话类型.        [只读]
    char             __padding__[2];
    char             passwordHash_r[20]; //// 会话口令的hash值.[只读]
    char             userData[64];       //// 用户自定义数据
};
#pragma pack(push, 1) //// [[[[
#pragma pack(pop)     //// ]]]]----------------------------------------------

/////////////////////////////////////////////////////////////////////////////
////

////
//// rtp包
////
#if !defined(____IRtpPacket____)
#define ____IRtpPacket____
class IRtpPacket
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    virtual void PRO_STDCALL SetMarker(bool m) = 0;
    
    virtual bool PRO_STDCALL GetMarker() const = 0;
    
    virtual void PRO_STDCALL SetPayloadType(char pt) = 0;
    
    virtual char PRO_STDCALL GetPayloadType() const = 0;
    
    virtual void PRO_STDCALL SetSequence(PRO_UINT16 seq) = 0;
    
    virtual PRO_UINT16 PRO_STDCALL GetSequence() const = 0;
    
    virtual void PRO_STDCALL SetTimeStamp(PRO_UINT32 ts) = 0;
    
    virtual PRO_UINT32 PRO_STDCALL GetTimeStamp() const = 0;
    
    virtual void PRO_STDCALL SetSsrc(PRO_UINT32 ssrc) = 0;
    
    virtual PRO_UINT32 PRO_STDCALL GetSsrc() const = 0;
    
    virtual void PRO_STDCALL SetMmId(PRO_UINT32 mmId) = 0;
    
    virtual PRO_UINT32 PRO_STDCALL GetMmId() const = 0;
    
    virtual void PRO_STDCALL SetMmType(RTP_MM_TYPE mmType) = 0;
    
    virtual RTP_MM_TYPE PRO_STDCALL GetMmType() const = 0;
    
    virtual void PRO_STDCALL SetKeyFrame(bool keyFrame) = 0;
    
    virtual bool PRO_STDCALL GetKeyFrame() const = 0;
    
    virtual void PRO_STDCALL SetFirstPacketOfFrame(bool firstPacket) = 0;
    
    virtual bool PRO_STDCALL GetFirstPacketOfFrame() const = 0;
    
    virtual const void* PRO_STDCALL GetPayloadBuffer() const = 0;
    
    virtual void* PRO_STDCALL GetPayloadBuffer() = 0;
    
    virtual PRO_UINT16 PRO_STDCALL GetPayloadSize() const = 0;
    
    virtual void PRO_STDCALL SetTick_i(PRO_INT64 tick) = 0;
    
    virtual PRO_INT64 PRO_STDCALL GetTick_i() const = 0;
};
#endif //// ____IRtpPacket____

/////////////////////////////////////////////////////////////////////////////
////

////
//// rtp会话
////
class IRtpSession
{
public:
    
    ////
    //// 获取会话信息
    ////
    virtual const RTP_SESSION_INFO PRO_STDCALL GetInfo() const = 0;
    
    ////
    //// 获取会话的套接字id
    ////
    //// 如非必须,最好不要直接操作底层的套接字
    ////
    virtual long PRO_STDCALL GetSockId() const = 0;
    
    ////
    //// 获取会话的本地ip地址
    ////
    virtual const char* PRO_STDCALL GetLocalIp(char localIp[64]) const = 0;
    
    ////
    //// 获取会话的本地端口号
    ////
    virtual unsigned short PRO_STDCALL GetLocalPort() const = 0;
    
    ////
    //// 获取会话的远端ip地址
    ////
    virtual const char* PRO_STDCALL GetRemoteIp(char remoteIp[64]) const = 0;
    
    ////
    //// 获取会话的远端端口号
    ////
    virtual unsigned short PRO_STDCALL GetRemotePort() const = 0;
    
    ////
    //// 设置会话的远端ip地址和端口号
    ////
    //// 仅用于RTP_ST_UDPCLIENT,RTP_ST_UDPSERVER类型的会话
    ////
    virtual void PRO_STDCALL SetRemoteIpAndPort(
        const char*    remoteIp,
        unsigned short remotePort
        ) = 0;
    
    ////
    //// 检查会话是否就绪
    ////
    virtual bool PRO_STDCALL IsReady() const = 0;
    
    ////
    //// 直接发送rtp包
    ////
    //// 如果返回false,表示发送池已满,上层负责缓冲以及重新发送
    ////
    virtual bool PRO_STDCALL SendPacket(IRtpPacket* packet) = 0;
    
    ////
    //// 通过定时器发送rtp包(for CRtpSessionWrapper only)
    ////
    //// 如果返回false,表示发送池已满,上层负责缓冲以及重新发送
    ////
    virtual bool PRO_STDCALL SendPacketByTimer(
        IRtpPacket*   packet,
        unsigned long sendDurationMs /* = 1 */
        ) = 0;
    
    ////
    //// 请求会话回调一个OnSend事件
    ////
    virtual void PRO_STDCALL RequestOnSend() = 0;
    
    ////
    //// rtp数据统计(for CRtpSessionWrapper only)----------------------------
    ////
    
    virtual float PRO_STDCALL GetInputBitRate() const = 0;
    
    virtual float PRO_STDCALL GetOutputBitRate() const = 0;
    
    virtual float PRO_STDCALL GetInputFrameRate() const = 0;
    
    virtual float PRO_STDCALL GetOutputFrameRate() const = 0;
    
    virtual float PRO_STDCALL GetInputLossRate() const = 0;
    
    virtual float PRO_STDCALL GetOutputLossRate() const = 0;
    
    virtual float PRO_STDCALL GetInputLossCount() const = 0;
    
    virtual float PRO_STDCALL GetOutputLossCount() const = 0;
    
    virtual void PRO_STDCALL ResetInputStat() = 0;
    
    virtual void PRO_STDCALL ResetOutputStat() = 0;
    
    ////
    //// rtp链路使能(for CRtpSessionWrapper only)----------------------------
    ////
    
    virtual void PRO_STDCALL EnableInput(bool enable) = 0;
    
    virtual void PRO_STDCALL EnableOutput(bool enable) = 0;
    
    ////
    //// rtp流量控制(for CRtpSessionWrapper only)----------------------------
    ////
    
    virtual void PRO_STDCALL SetRedlineBytes(unsigned long redlineBytes) = 0;
    
    virtual void PRO_STDCALL SetRedlineFrames(unsigned long redlineFrames) = 0;
};

////
//// rtp会话回调目标
////
//// 使用者需要实现该接口
////
class IRtpSessionObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 状态就绪时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnOkSession(IRtpSession* session) = 0;
    
    ////
    //// rtp包抵达时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnRecvSession(
        IRtpSession* session,
        IRtpPacket*  packet
        ) = 0;
    
    ////
    //// 发送能力空闲时,该函数将被回调
    ////
    virtual void PRO_STDCALL OnSendSession(
        IRtpSession* session,
        bool         packetErased
        ) = 0;
    
    ////
    //// 出现错误时,该函数将被回调
    ////
    //// errorCode为系统错误码
    ////
    virtual void PRO_STDCALL OnCloseSession(
        IRtpSession* session,
        long         errorCode
        ) = 0;
};

/////////////////////////////////////////////////////////////////////////////
////

////
//// rtp接受器回调目标
////
//// 使用者需要实现该接口
////
class IRtpAcceptorObserver
{
public:
    
    virtual unsigned long PRO_STDCALL AddRef() = 0;
    
    virtual unsigned long PRO_STDCALL Release() = 0;
    
    ////
    //// 有会话进入时,该函数将被回调
    ////
    //// 使用者对remoteInfo进行解析校验后,应该根据remoteInfo的会话类型把sockId包装成
    //// RTP_ST_TCPSERVER_EX或RTP_ST_SSLSERVER_EX类型的(IRtpSession*),
    //// 或释放sockId对应的套接字资源
    ////
    //// 注意!!! remoteInfo是远端发过来的远端自己的会话信息
    ////
    virtual void PRO_STDCALL OnAcceptSession(
        IRtpAcceptor*           acceptor,
        long                    sockId, //// -1无效,接受失败
        const RTP_SESSION_INFO* remoteInfo,
        PRO_UINT32              token
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
RTP_FRAMEWORK_API
void
PRO_STDCALL
InitRtpFramework();

////
//// 功能: 创建一个rtp包
////
//// 参数:
//// payloadBuffer : 媒体数据指针
//// payloadSize   : 媒体数据长度.最多(1024 * 60)字节
////
//// 返回值: rtp包对象或空
////
//// 说明: 使用者应该继续初始化rtp包的头部字段
////
RTP_FRAMEWORK_API
IRtpPacket*
PRO_STDCALL
CreateRtpPacket(const void* payloadBuffer,
                PRO_UINT16  payloadSize);

////
//// 功能: 创建一个rtp包
////
//// 参数:
//// payloadSize : 媒体数据长度.最多(1024 * 60)字节
////
//// 返回值: rtp包对象或空
////
//// 说明: 该版本主要用于减少内存拷贝次数.
////       例如,编码器可以通过IRtpPacket::GetPayloadBuffer(...)得到媒体数据指针,
////       然后直接进行媒体数据的初始化等操作
////
RTP_FRAMEWORK_API
IRtpPacket*
PRO_STDCALL
CreateRtpPacketSpace(PRO_UINT16 payloadSize);

////
//// 功能: 克隆一个rtp包
////
//// 参数:
//// packet : 原始的rtp包对象
////
//// 返回值: 克隆的rtp包对象或空
////
//// 说明: 无
////
RTP_FRAMEWORK_API
IRtpPacket*
PRO_STDCALL
CloneRtpPacket(const IRtpPacket* packet);

////
//// 功能: 解析一段标准的rtp流
////
//// 参数:
//// streamBuffer : 流指针
//// streamSize   : 流长度
////
//// 返回值: rtp包对象或空
////
//// 说明: 解析过程会忽略IRtpPacket不支持的字段
////
RTP_FRAMEWORK_API
IRtpPacket*
PRO_STDCALL
ParseRtpStreamToPacket(const void* streamBuffer,
                       PRO_UINT16  streamSize);

////
//// 功能: 查找一段标准的rtp流
////
//// 参数:
//// packet     : rtp包对象
//// streamSize : 流长度
////
//// 返回值: 流指针或空
////
//// 说明: 返回时,(*streamSize)指示了该rtp流的长度
////
RTP_FRAMEWORK_API
const void*
PRO_STDCALL
FindRtpStreamFromPacket(const IRtpPacket* packet,
                        PRO_UINT16*       streamSize);

////
//// 功能: 设置rtp端口号的分配范围
////
//// 参数:
//// minUdpPort : 最小udp端口号
//// maxUdpPort : 最大udp端口号
//// minTcpPort : 最小tcp端口号
//// maxTcpPort : 最大tcp端口号
////
//// 返回值: 无
////
//// 说明: 默认的端口号分配范围为(3000 ~ 3999).偶数开始,奇数结束
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
SetRtpPortRange(unsigned short minUdpPort,
                unsigned short maxUdpPort,
                unsigned short minTcpPort,
                unsigned short maxTcpPort);

////
//// 功能: 获取rtp端口号的分配范围
////
//// 参数:
//// minUdpPort : 用于接收最小udp端口号.可以为NULL
//// maxUdpPort : 用于接收最大udp端口号.可以为NULL
//// minTcpPort : 用于接收最小tcp端口号.可以为NULL
//// maxTcpPort : 用于接收最大tcp端口号.可以为NULL
////
//// 返回值: 无
////
//// 说明: 默认的端口号分配范围为(3000 ~ 3999).偶数开始,奇数结束
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
GetRtpPortRange(unsigned short* minUdpPort,
                unsigned short* maxUdpPort,
                unsigned short* minTcpPort,
                unsigned short* maxTcpPort);

////
//// 功能: 自动分配一个udp端口号
////
//// 参数: 无
////
//// 返回值: udp端口号.[偶数]
////
//// 说明: 返回的端口号不一定空闲,应该多次分配尝试
////
RTP_FRAMEWORK_API
unsigned short
PRO_STDCALL
AllocRtpUdpPort();

////
//// 功能: 自动分配一个tcp端口号
////
//// 参数: 无
////
//// 返回值: tcp端口号.[偶数]
////
//// 说明: 返回的端口号不一定空闲,应该多次分配尝试
////
RTP_FRAMEWORK_API
unsigned short
PRO_STDCALL
AllocRtpTcpPort();

////
//// 功能: 设置会话的心跳周期
////
//// 参数:
//// heartbeatInSeconds : 心跳周期.默认10秒
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
SetRtpHeartbeatInterval(unsigned long heartbeatInSeconds = 0);

////
//// 功能: 获取会话的心跳周期
////
//// 参数: 无
////
//// 返回值: 心跳周期
////
//// 说明: 无
////
RTP_FRAMEWORK_API
unsigned long
PRO_STDCALL
GetRtpHeartbeatInterval();

////
//// 功能: 设置会话的保活周期
////
//// 参数:
//// keepaliveInSeconds : 保活周期.默认180秒
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
SetRtpKeepaliveInterval(unsigned long keepaliveInSeconds = 0);

////
//// 功能: 获取会话的保活周期
////
//// 参数: 无
////
//// 返回值: 保活周期
////
//// 说明: 无
////
RTP_FRAMEWORK_API
unsigned long
PRO_STDCALL
GetRtpKeepaliveInterval();

////
//// 功能: 设置会话的统计时间窗
////
//// 参数:
//// statInSeconds : 统计时间窗.默认5秒
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
SetRtpStatTimeSpan(unsigned long statInSeconds = 0);

////
//// 功能: 获取会话的统计时间窗
////
//// 参数: 无
////
//// 返回值: 统计时间窗
////
//// 说明: 无
////
RTP_FRAMEWORK_API
unsigned long
PRO_STDCALL
GetRtpStatTimeSpan();

////
//// 功能: 创建一个RTP_ST_UDPCLIENT类型的会话
////
//// 参数:
//// observer  : 回调目标
//// reactor   : 反应器
//// localInfo : 会话信息
//// antiNat   : 是否向NAT内部反向推送数据
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort : 要绑定的端口号.[偶数].如果为0,系统将随机分配一个端口号
////
//// 返回值: 会话对象或空
////
//// 说明: 可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionUdpclient(IRtpSessionObserver*    observer,
                          IProReactor*            reactor,
                          const RTP_SESSION_INFO* localInfo,
                          bool                    antiNat,
                          const char*             localIp   = NULL,
                          unsigned short          localPort = 0);

////
//// 功能: 创建一个RTP_ST_UDPSERVER类型的会话
////
//// 参数:
//// observer  : 回调目标
//// reactor   : 反应器
//// localInfo : 会话信息
//// antiNat   : 是否向NAT内部反向推送数据
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort : 要绑定的端口号.[偶数].如果为0,系统将随机分配一个端口号
////
//// 返回值: 会话对象或空
////
//// 说明: 可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionUdpserver(IRtpSessionObserver*    observer,
                          IProReactor*            reactor,
                          const RTP_SESSION_INFO* localInfo,
                          bool                    antiNat,
                          const char*             localIp   = NULL,
                          unsigned short          localPort = 0);

////
//// 功能: 创建一个RTP_ST_TCPCLIENT类型的会话
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// localInfo        : 会话信息
//// remoteIp         : 远端的ip地址或域名
//// remotePort       : 远端的端口号
//// localIp          : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: 会话对象或空
////
//// 说明: 无
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionTcpclient(IRtpSessionObserver*    observer,
                          IProReactor*            reactor,
                          const RTP_SESSION_INFO* localInfo,
                          const char*             remoteIp,
                          unsigned short          remotePort,
                          const char*             localIp          = NULL,
                          unsigned long           timeoutInSeconds = 0);

////
//// 功能: 创建一个RTP_ST_TCPSERVER类型的会话
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// localInfo        : 会话信息
//// localIp          : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort        : 要绑定的端口号.[偶数].如果为0,系统将随机分配一个端口号
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: 会话对象或空
////
//// 说明: 可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionTcpserver(IRtpSessionObserver*    observer,
                          IProReactor*            reactor,
                          const RTP_SESSION_INFO* localInfo,
                          const char*             localIp          = NULL,
                          unsigned short          localPort        = 0,
                          unsigned long           timeoutInSeconds = 0);

////
//// 功能: 创建一个RTP_ST_UDPCLIENT_EX类型的会话
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// localInfo        : 会话信息
//// remoteIp         : 远端的ip地址或域名
//// remotePort       : 远端的端口号
//// localIp          : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: 会话对象或空
////
//// 说明: 无
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionUdpclientEx(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            const char*             remoteIp,
                            unsigned short          remotePort,
                            const char*             localIp          = NULL,
                            unsigned long           timeoutInSeconds = 0);

////
//// 功能: 创建一个RTP_ST_UDPSERVER_EX类型的会话
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// localInfo        : 会话信息
//// localIp          : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort        : 要绑定的端口号.[偶数].如果为0,系统将随机分配一个端口号
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: 会话对象或空
////
//// 说明: 可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionUdpserverEx(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            const char*             localIp          = NULL,
                            unsigned short          localPort        = 0,
                            unsigned long           timeoutInSeconds = 0);

////
//// 功能: 创建一个RTP_ST_TCPCLIENT_EX类型的会话
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// localInfo        : 会话信息
//// remoteIp         : 远端的ip地址或域名
//// remotePort       : 远端的端口号
//// localIp          : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// password         : 会话口令
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: 会话对象或空
////
//// 说明: 无
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionTcpclientEx(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            const char*             remoteIp,
                            unsigned short          remotePort,
                            const char*             localIp          = NULL,
                            const char*             password         = NULL,
                            unsigned long           timeoutInSeconds = 0);

////
//// 功能: 创建一个RTP_ST_TCPSERVER_EX类型的会话
////
//// 参数:
//// observer  : 回调目标
//// reactor   : 反应器
//// localInfo : 会话信息.根据IRtpAcceptorObserver::OnAcceptSession(...)的remoteInfo解析产生
//// sockId    : 套接字id.来自于IRtpAcceptorObserver::OnAcceptSession(...)
////
//// 返回值: 会话对象或空
////
//// 说明: 无
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionTcpserverEx(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            long                    sockId);

////
//// 功能: 创建一个RTP_ST_SSLCLIENT_EX类型的会话
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// localInfo        : 会话信息
//// remoteIp         : 远端的ip地址或域名
//// remotePort       : 远端的端口号
//// localIp          : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// password         : 会话口令
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: 会话对象或空
////
//// 说明: 无
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionSslclientEx(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            const char*             remoteIp,
                            unsigned short          remotePort,
                            const char*             localIp          = NULL,
                            const char*             password         = NULL,
                            unsigned long           timeoutInSeconds = 0);

////
//// 功能: 创建一个RTP_ST_SSLSERVER_EX类型的会话
////
//// 参数:
//// observer        : 回调目标
//// reactor         : 反应器
//// localInfo       : 会话信息.根据IRtpAcceptorObserver::OnAcceptSession(...)的remoteInfo解析产生
//// sockId          : 套接字id.来自于IRtpAcceptorObserver::OnAcceptSession(...)
//// xorOnly         : 是否仅使用异或运算
//// sessionKeyBytes : 会话密钥的字节数(16/32/64).默认16字节(128位)
////
//// 返回值: 会话对象或空
////
//// 说明: 无
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionSslserverEx(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            long                    sockId,
                            bool                    xorOnly,
                            unsigned char           sessionKeyBytes = 0);

////
//// 功能: 创建一个RTP_ST_MCASTCLIENT类型的会话
////
//// 参数:
//// observer  : 回调目标
//// reactor   : 反应器
//// localInfo : 会话信息
//// mcastIp   : 要绑定的多播地址
//// mcastPort : 要绑定的多播端口号.[偶数].如果为0,系统将随机分配一个端口号
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
////
//// 返回值: 会话对象或空
////
//// 说明: 合法的多播地址为(224.0.0.0 ~ 239.255.255.255),
////       推荐的多播地址为(224.0.1.0 ~ 238.255.255.255),
////       RFC-1112(IGMPv1), RFC-2236(IGMPv2), RFC-3376(IGMPv3)
////
////       可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionMcastclient(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            const char*             mcastIp,
                            unsigned short          mcastPort = 0,
                            const char*             localIp   = NULL);

////
//// 功能: 创建一个RTP_ST_MCASTSERVER类型的会话
////
//// 参数:
//// observer  : 回调目标
//// reactor   : 反应器
//// localInfo : 会话信息
//// mcastIp   : 要绑定的多播地址
//// mcastPort : 要绑定的多播端口号.[偶数].如果为0,系统将随机分配一个端口号
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
////
//// 返回值: 会话对象或空
////
//// 说明: 合法的多播地址为(224.0.0.0 ~ 239.255.255.255),
////       推荐的多播地址为(224.0.1.0 ~ 238.255.255.255),
////       RFC-1112(IGMPv1), RFC-2236(IGMPv2), RFC-3376(IGMPv3)
////
////       可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionMcastserver(IRtpSessionObserver*    observer,
                            IProReactor*            reactor,
                            const RTP_SESSION_INFO* localInfo,
                            const char*             mcastIp,
                            unsigned short          mcastPort = 0,
                            const char*             localIp   = NULL);

////
//// 功能: 创建一个RTP_ST_MCASTCLIENT_EX类型的会话
////
//// 参数:
//// observer  : 回调目标
//// reactor   : 反应器
//// localInfo : 会话信息
//// mcastIp   : 要绑定的多播地址
//// mcastPort : 要绑定的多播端口号.[偶数].如果为0,系统将随机分配一个端口号
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
////
//// 返回值: 会话对象或空
////
//// 说明: 合法的多播地址为(224.0.0.0 ~ 239.255.255.255),
////       推荐的多播地址为(224.0.1.0 ~ 238.255.255.255),
////       RFC-1112(IGMPv1), RFC-2236(IGMPv2), RFC-3376(IGMPv3)
////
////       可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionMcastclientEx(IRtpSessionObserver*    observer,
                              IProReactor*            reactor,
                              const RTP_SESSION_INFO* localInfo,
                              const char*             mcastIp,
                              unsigned short          mcastPort = 0,
                              const char*             localIp   = NULL);

////
//// 功能: 创建一个RTP_ST_MCASTSERVER_EX类型的会话
////
//// 参数:
//// observer  : 回调目标
//// reactor   : 反应器
//// localInfo : 会话信息
//// mcastIp   : 要绑定的多播地址
//// mcastPort : 要绑定的多播端口号.[偶数].如果为0,系统将随机分配一个端口号
//// localIp   : 要绑定的本地ip地址.如果为NULL,系统将使用0.0.0.0
////
//// 返回值: 会话对象或空
////
//// 说明: 合法的多播地址为(224.0.0.0 ~ 239.255.255.255),
////       推荐的多播地址为(224.0.1.0 ~ 238.255.255.255),
////       RFC-1112(IGMPv1), RFC-2236(IGMPv2), RFC-3376(IGMPv3)
////
////       可以使用IRtpSession::GetLocalPort(...)获取本地的端口号
////
RTP_FRAMEWORK_API
IRtpSession*
PRO_STDCALL
CreateRtpSessionMcastserverEx(IRtpSessionObserver*    observer,
                              IProReactor*            reactor,
                              const RTP_SESSION_INFO* localInfo,
                              const char*             mcastIp,
                              unsigned short          mcastPort = 0,
                              const char*             localIp   = NULL);

////
//// 功能: 删除一个会话
////
//// 参数:
//// session : 会话对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
DeleteRtpSession(IRtpSession* session);

////
//// 功能: 创建一个rtp接受器
////
//// 参数:
//// observer         : 回调目标
//// reactor          : 反应器
//// localIp          : 要监听的本地ip地址.如果为NULL,系统将使用0.0.0.0
//// localPort        : 要监听的端口号.如果为0,系统将随机分配一个端口号
//// timeoutInSeconds : 握手超时.默认60秒
////
//// 返回值: rtp接受器对象或空
////
//// 说明: 可以使用GetRtpAcceptorPort(...)获取实际的端口号
////
RTP_FRAMEWORK_API
IRtpAcceptor*
PRO_STDCALL
CreateRtpAcceptor(IRtpAcceptorObserver* observer,
                  IProReactor*          reactor,
                  const char*           localIp          = NULL,
                  unsigned short        localPort        = 0,
                  unsigned long         timeoutInSeconds = 0);

////
//// 功能: 获取rtp接受器监听的端口号
////
//// 参数:
//// acceptor : rtp接受器对象
////
//// 返回值: 端口号
////
//// 说明: 主要用于随机监听的端口号的获取
////
RTP_FRAMEWORK_API
unsigned short
PRO_STDCALL
GetRtpAcceptorPort(IRtpAcceptor* acceptor);

////
//// 功能: 删除一个rtp接受器
////
//// 参数:
//// acceptor : rtp接受器对象
////
//// 返回值: 无
////
//// 说明: 无
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
DeleteRtpAcceptor(IRtpAcceptor* acceptor);

////
//// 功能: 计算口令的hash-20值
////
//// 参数:
//// password     : 口令
//// token        : 随机令牌
//// passwordHash : 口令的hash-20结果
////
//// 返回值: 无
////
//// 说明: 计算准则为 passwordHash = hash(password + token)
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
CalcRtpPasswordHash20(const char* password,
                      PRO_UINT32  token,
                      char        passwordHash[20]);

////
//// 功能: 计算口令的hash-40值
////
//// 参数:
//// password     : 口令
//// token        : 随机令牌
//// passwordHash : 口令的hash-40结果
////
//// 返回值: 无
////
//// 说明: 计算准则为 passwordHash = hash(password + token) ---> "%02x"
////
RTP_FRAMEWORK_API
void
PRO_STDCALL
CalcRtpPasswordHash40(const char* password,
                      PRO_UINT32  token,
                      char        passwordHash[40]);

////
//// 功能: 校验口令的hash-20值
////
//// 参数:
//// password     : 口令
//// token        : 随机令牌
//// passwordHash : 口令的hash-20值
////
//// 返回值: true匹配,false不匹配
////
//// 说明: IRtpAcceptorObserver::OnAcceptSession(...)会用到该函数.
////       校验准则为 hash(password + token) == passwordHash
////
RTP_FRAMEWORK_API
bool
PRO_STDCALL
CheckRtpPasswordHash20(const char* password,
                       PRO_UINT32  token,
                       const char  passwordHash[20]);

////
//// 功能: 校验口令的hash-40值
////
//// 参数:
//// password     : 口令
//// token        : 随机令牌
//// passwordHash : 口令的hash-40值
////
//// 返回值: true匹配,false不匹配
////
//// 说明: IRtpAcceptorObserver::OnAcceptSession(...)会用到该函数.
////       校验准则为 hash(password + token) ---> "%02x" == passwordHash
////
RTP_FRAMEWORK_API
bool
PRO_STDCALL
CheckRtpPasswordHash40(const char* password,
                       PRO_UINT32  token,
                       const char  passwordHash[40]);

/////////////////////////////////////////////////////////////////////////////
////

PRO_EXTERN_C2

#endif //// ____RTP_FRAMEWORK_H____
