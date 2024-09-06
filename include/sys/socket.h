#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#include <bits/utils.h>
#include <sys/types.h>
#include <bits/iov.h>

__begin_decls

typedef unsigned short sa_family_t;
typedef uint32_t socklen_t;

struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
};

struct sockaddr_storage {
	sa_family_t sa_family;
	char __padding[128 - sizeof(sa_family_t) - sizeof(unsigned long)];
	unsigned long __align;
};

struct msghdr {
	void* msg_name;
	socklen_t msg_namelen;
	struct iovec* msg_iov;
	size_t msg_iovlen;
	void* msg_control;
	size_t msg_controllen;
	int msg_flags;
};

struct cmsghdr {
	size_t cmsg_len;
	int cmsg_level;
	int cmsg_type;
};

#define MSG_OOB 1
#define MSG_PEEK 2
#define MSG_DONTROUTE 4
#define MSG_TRYHARD MSG_DONTROUTE
#define MSG_CTRUNC 8
#define MSG_PROXY 0x10
#define MSG_TRUNC 0x20
#define MSG_DONTWAIT 0x40
#define MSG_EOR 0x80
#define MSG_WAITALL 0x100
#define MSG_FIN 0x200
#define MSG_SYN 0x400
#define MSG_CONFIRM 0x800
#define MSG_RST 0x1000
#define MSG_ERRQUEUE 0x2000
#define MSG_NOSIGNAL 0x4000
#define MSG_MORE 0x8000
#define MSG_WAITFORONE 0x10000
#define MSG_BATCH 0x40000
#define MSG_ZEROCOPY 0x4000000
#define MSG_FASTOPEN 0x20000000
#define MSG_CMSG_CLOEXEC 0x40000000

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_RDM 4
#define SOCK_SEQPACKET 5
#define SOCK_DCCP 6

#define SOCK_NONBLOCK 0x800
#define SOCK_CLOEXEC 0x80000

#define PF_UNSPEC 0
#define PF_LOCAL 1
#define PF_UNIX PF_LOCAL
#define PF_FILE PF_LOCAL
#define PF_INET 2
#define PF_INET6 10
#define PF_NETLINK 16
#define PF_PACKET 17

#define AF_UNSPEC PF_UNSPEC
#define AF_LOCAL PF_LOCAL
#define AF_UNIX PF_UNIX
#define AF_FILE PF_FILE
#define AF_INET PF_INET
#define AF_INET6 PF_INET6
#define AF_NETLINK PF_NETLINK
#define AF_PACKET PF_PACKET

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

#define SOL_SOCKET 1

#define SO_DEBUG 1
#define SO_REUSEADDR 2
#define SO_TYPE 3
#define SO_ERROR 4
#define SO_DONTROUTE 5
#define SO_BROADCAST 6
#define SO_SNDBUF 7
#define SO_RCVBUF 8
#define SO_KEEPALIVE 9
#define SO_OOBINLINE 10
#define SO_NO_CHECK 11
#define SO_PRIORITY 12
#define SO_LINGER 13
#define SO_BSDCOMPAT 14
#define SO_REUSEPORT 15
#define SO_PASSCRED	16
#define SO_PEERCRED	17
#define SO_RCVLOWAT	18
#define SO_SNDLOWAT	19
#define SO_RCVTIMEO_OLD	20
#define SO_SNDTIMEO_OLD	21
#define SO_SNDBUFFORCE 32
#define SO_RCVBUFFORCE 33
#define SO_RCVTIMEO_NEW         66
#define SO_SNDTIMEO_NEW         67

#if UINTPTR_MAX == UINT64_MAX

#define SO_RCVTIMEO SO_RCVTIMEO_OLD
#define SO_SNDTIMEO SO_SNDTIMEO_OLD

#else

#define SO_RCVTIMEO (sizeof(time_t) == sizeof(long) ? SO_RCVTIMEO_OLD : SO_RCVTIMEO_NEW)
#define SO_SNDTIMEO (sizeof(time_t) == sizeof(long) ? SO_SNDTIMEO_OLD : SO_SNDTIMEO_NEW)

#endif

int socket(int __domain, int __type, int __protocol);
int socketpair(int __domain, int __type, int __protocol, int __sv[2]);
int connect(int __fd, const struct sockaddr* __addr, socklen_t __addr_len);
int bind(int __fd, const struct sockaddr* __addr, socklen_t __addr_len);
int listen(int __fd, int __backlog);
int accept(int __fd, struct sockaddr* __restrict __addr, socklen_t* __restrict __addr_len);
int shutdown(int __fd, int __how);

int getsockopt(int __fd, int __level, int __option, void* __restrict __value, socklen_t* __restrict __value_len);
int setsockopt(int __fd, int __level, int __option, const void* __value, socklen_t __value_len);

int getsockname(int __fd, struct sockaddr* __restrict __addr, socklen_t* __restrict __addr_len);
int getpeername(int __fd, struct sockaddr* __restrict __addr, socklen_t* __restrict __addr_len);

int sockatmark(int __fd);

ssize_t send(int __fd, const void* __buf, size_t __len, int __flags);
ssize_t sendto(
	int __fd,
	const void* __buf,
	size_t __len,
	int __flags,
	const struct sockaddr* __dest_addr,
	socklen_t __addr_len);
ssize_t sendmsg(int __fd, const struct msghdr* __msg, int __flags);

ssize_t recv(int __fd, void* __restrict __buf, size_t __len, int __flags);
ssize_t recvfrom(
	int __fd,
	void* __restrict __buf,
	size_t __len,
	int __flags,
	struct sockaddr* __restrict __src_addr,
	socklen_t* __restrict __addr_len);
ssize_t recvmsg(int __fd, struct msghdr* __msg, int __flags);

// linux
int accept4(int __fd, struct sockaddr* __restrict __addr, socklen_t* __restrict __addr_len, int __flags);

__end_decls

#endif
