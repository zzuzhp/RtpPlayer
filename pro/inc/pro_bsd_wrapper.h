#if !defined(____PRO_BSD_WRAPPER_H____)
#define ____PRO_BSD_WRAPPER_H____

#include "pro_a.h"
#include "pro_extern_c.h"
#include "pro_z.h" //// for <cerrno>

#if defined(WIN32) || defined(_WIN32_WCE) || defined(__TI_COMPILER_VERSION__)
#if !defined(FD_SETSIZE)
#define FD_SETSIZE 1024
#endif
#endif

#if defined(WIN32) || defined(_WIN32_WCE)
#define WIN32_LEAN_AND_MEAN
#include <windows.h> //// for (_WIN32_WINNT>=0x0500)!!!
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#elif defined(__TI_COMPILER_VERSION__)
#include <ti/ndk/inc/netmain.h>
#include <ti/ndk/inc/stacksys.h>
#if (FD_SETSIZE < 1024)
#error "FD_SETSIZE < 1024, ti/ndk/inc/socket.h"
#endif
#else
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#if defined(_MSC_VER)
#if defined(_WIN32_WCE)
#pragma comment(lib, "ws2.lib")
#elif defined(WIN32)
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "ws2_32.lib")
#endif
#endif

PRO_EXTERN_C1

/////////////////////////////////////////////////////////////////////////////
////

#if defined(WIN32) || defined(_WIN32_WCE)
#define PBSD_EINTR             WSAEINTR       //// 10004
#define PBSD_EWOULDBLOCK       WSAEWOULDBLOCK //// 10035
#define PBSD_ECONNRESET        WSAECONNRESET  //// 10054
#define PBSD_ETIMEDOUT         WSAETIMEDOUT   //// 10060
#elif defined(__TI_COMPILER_VERSION__)
#define PBSD_EINTR             EINTR          //// 4
#define PBSD_EWOULDBLOCK       EWOULDBLOCK    //// 35
#define PBSD_ECONNRESET        ECONNRESET     //// 54
#define PBSD_ETIMEDOUT         ETIMEDOUT      //// 60
#else
#define PBSD_EINTR             EINTR          //// 4
#define PBSD_EWOULDBLOCK       EAGAIN         //// 11
#define PBSD_ECONNRESET        ECONNRESET     //// 104
#define PBSD_ETIMEDOUT         ETIMEDOUT      //// 110
#endif

#if defined(WIN32) || defined(_WIN32_WCE)
#define PBSD_FD_ZERO(set)      FD_ZERO(set)
#define PBSD_FD_SET(fd, set)   FD_SET(((SOCKET)(fd)), set)
#define PBSD_FD_CLR(fd, set)   FD_CLR(((SOCKET)(fd)), set)
#define PBSD_FD_ISSET(fd, set) FD_ISSET(((SOCKET)(fd)), set)
#elif defined(__TI_COMPILER_VERSION__)
#define PBSD_FD_ZERO(set)      FD_ZERO(set)
#define PBSD_FD_SET(fd, set)   FD_SET(((HANDLE)(fd)), set)
#define PBSD_FD_CLR(fd, set)   FD_CLR(((HANDLE)(fd)), set)
#define PBSD_FD_ISSET(fd, set) FD_ISSET(((HANDLE)(fd)), set)
#else
#define PBSD_FD_ZERO(set)      FD_ZERO(set)
#define PBSD_FD_SET(fd, set)   FD_SET(((int)(fd)), set)
#define PBSD_FD_CLR(fd, set)   FD_CLR(((int)(fd)), set)
#define PBSD_FD_ISSET(fd, set) FD_ISSET(((int)(fd)), set)
#endif

#if defined(WIN32) || defined(_WIN32_WCE)
#if !defined(SO_EXCLUSIVEADDRUSE)
#define SO_EXCLUSIVEADDRUSE    ((int)(~SO_REUSEADDR))
#endif
#if !defined(SIO_UDP_CONNRESET)
#define SIO_UDP_CONNRESET      _WSAIOW(IOC_VENDOR, 12)
#endif
#if !defined(MSG_DONTWAIT)
#define MSG_DONTWAIT           0
#endif
#elif defined(__TI_COMPILER_VERSION__)
#if !defined(FIONBIO)
#define FIONBIO                ((int)(~SO_BLOCKING))
#endif
#endif

static const int PBSD_FD_SETSIZE = FD_SETSIZE;

/////////////////////////////////////////////////////////////////////////////
////

void
PRO_STDCALL
pbsd_startup();

int
PRO_STDCALL
pbsd_errno();

int
PRO_STDCALL
pbsd_gethostname(char* name, int namelen);

unsigned long
PRO_STDCALL
pbsd_inet_aton(const char* ipornamestring);

const char*
PRO_STDCALL
pbsd_inet_ntoa(struct in_addr in, char ipstring[64]);

unsigned long
PRO_STDCALL
pbsd_htonl(unsigned long hostlong);

unsigned long
PRO_STDCALL
pbsd_ntohl(unsigned long netlong);

unsigned short
PRO_STDCALL
pbsd_htons(unsigned short hostshort);

unsigned short
PRO_STDCALL
pbsd_ntohs(unsigned short netshort);

long
PRO_STDCALL
pbsd_socket(int af, int type, int protocol);

int
PRO_STDCALL
pbsd_socketpair(long fds[2]);

int
PRO_STDCALL
pbsd_bind(long fd, const struct sockaddr_in* addr, bool reuseaddr);

int
PRO_STDCALL
pbsd_listen(long fd);

long
PRO_STDCALL
pbsd_accept(long fd, struct sockaddr_in* addr);

int
PRO_STDCALL
pbsd_connect(long fd, const struct sockaddr_in* addr);

int
PRO_STDCALL
pbsd_send(long fd, const void* buf, int buflen, int flags);

int
PRO_STDCALL
pbsd_recv(long fd, void* buf, int buflen, int flags);

int
PRO_STDCALL
pbsd_sendto(long fd, const void* buf, int buflen, int flags, const struct sockaddr_in* addr);

int
PRO_STDCALL
pbsd_recvfrom(long fd, void* buf, int buflen, int flags, struct sockaddr_in* addr);

int
PRO_STDCALL
pbsd_select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);

int
PRO_STDCALL
pbsd_ioctlsocket(long fd, int cmd, int* arg);

int
PRO_STDCALL
pbsd_setsockopt(long fd, int level, int optname, const void* optval, int optlen);

int
PRO_STDCALL
pbsd_getsockopt(long fd, int level, int optname, void* optval, int* optlen);

int
PRO_STDCALL
pbsd_getsockname(long fd, struct sockaddr_in* addr);

int
PRO_STDCALL
pbsd_getpeername(long fd, struct sockaddr_in* addr);

void
PRO_STDCALL
pbsd_shutdown(long fd);

void
PRO_STDCALL
pbsd_closesocket(long fd);

/////////////////////////////////////////////////////////////////////////////
////

const char*
PRO_STDCALL
ProGetLocalFirstIp(char        localFirstIp[64],
                   const char* peerIp = NULL);

unsigned long
PRO_STDCALL
ProGetLocalIpList(char localIpList[8][64]);

bool
PRO_STDCALL
ProCheckIpFormat(const char* ipString);

/////////////////////////////////////////////////////////////////////////////
////

PRO_EXTERN_C2

#endif //// ____PRO_BSD_WRAPPER_H____
