/*
 *  platform setting definition
 */

#ifndef __PLATFORM_H
#define __PLATFORM_H

#if defined(linux)
#define LINUX     1
#define OSX       0
#define WIN       0
#define FREEBSD   0
#elif defined (__APPLE__)
#define LINUX     0
#define OSX       1
#define WIN       0
#define FREEBSD   0
#elif defined (_MSC_VER) || defined(__BORLANDC__)
#define LINUX     0
#define OSX       0
#define WIN       1
#define FREEBSD   0
#elif defined(__FreeBSD__)
#define LINUX     0
#define OSX       0
#define WIN       0
#define FREEBSD   1
#else
#error unknown target
#endif

#include <stdbool.h>
#include <signal.h>
#include <sys/stat.h>

#define _STR_LEN_ 256

#if LINUX || OSX || FREEBSD
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/poll.h>
#include <poll.h>
#include <dlfcn.h>
#include <pthread.h>
#include <errno.h>
//#include <memcheck.h>
#include <strings.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

typedef u_int8_t  u8_t;
typedef u_int16_t u16_t;
typedef u_int32_t u32_t;
typedef u_int64_t u64_t;
typedef int16_t   s16_t;
typedef int32_t   s32_t;
typedef int64_t   s64_t;

#define last_error() errno
#define ERROR_WOULDBLOCK EWOULDBLOCK

int SendARP(in_addr_t src, in_addr_t dst, u8_t mac[], unsigned long *size);
#define fresize(f,s) ftruncate(fileno(f), s)
char *strlwr(char *str);
#define _random(x) random()
char *GetTempPath(u16_t size, char *path);
int asprintf(char **strp, const char *fmt, ...);
#define S_ADDR(X) X.s_addr

#endif

#if WIN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <sys/timeb.h>

typedef unsigned __int8  u8_t;
typedef unsigned __int16 u16_t;
typedef unsigned __int32 u32_t;
typedef unsigned __int64 u64_t;
typedef __int16 s16_t;
typedef __int32 s32_t;
typedef __int64 s64_t;

#define inline __inline

int gettimeofday(struct timeval *tv, struct timezone *tz);
int asprintf(char **strp, const char *fmt, ...);

//#define poll(fds,numfds,timeout) WSAPoll(fds,numfds,timeout)

#define sleep(x) Sleep((x)*1000)
#define last_error() WSAGetLastError()
#define ERROR_WOULDBLOCK WSAEWOULDBLOCK
#define open _open
#define read _read
#define snprintf _snprintf
#define fresize(f, s) chsize(fileno(f), s)
#define strcasecmp stricmp
#define _random(x) random(x)
#define VALGRIND_MAKE_MEM_DEFINED(x,y)
#define S_ADDR(X) X.S_un.S_addr

#define in_addr_t u32_t
#define socklen_t int
#define ssize_t int

#define RTLD_NOW 0

#endif

typedef struct ntp_s {
	u32_t seconds;





u32_t gettime_ms(void);
u64_t gettime_ms64(void);

#define SL_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

