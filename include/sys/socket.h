#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#include <bits/utils.h>
#include <stdint.h>
#include <stddef.h>

__begin

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

struct iovec {
	void* iov_base;
	size_t iov_len;
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

int socket(int __domain, int __type, int __protocol);

__end

#endif
