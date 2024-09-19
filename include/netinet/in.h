#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <bits/utils.h>
#include <sys/socket.h>

__begin_decls

#define IPPROTO_IP 0
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

#define INADDR_ANY ((in_addr_t) 0)
#define INADDR_LOOPBACK ((in_addr_t) 0x7F000001)
#define INADDR_BROADCAST ((in_addr_t) 0xFFFFFFFF)
#define INADDR_NONE ((in_addr_t) 0xFFFFFFFF)

#define IN6ADDR_LOOPBACK_INIT {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}}}
#define IN6ADDR_ANY_INIT {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}

#define __ARE_4BYTE_EQUAL(a, b) ((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2] && (a)[3] == (b)[3])
#define IN6_ARE_ADDR_EQUAL(a, b) __ARE_4BYTE_EQUAL((const uint32_t*) (a), (const uint32_t*) (b))
# define IN6_IS_ADDR_LOOPBACK(a) \
	(((const uint32_t*) (a))[0] == 0 && \
	((const uint32_t*) (a))[1] == 0 && \
	((const uint32_t*) (a))[2] == 0 && \
	((const uint32_t*) (a))[3] == htonl (1))
# define IN6_IS_ADDR_V4MAPPED(a) \
	((((const uint32_t*) (a))[0] == 0) && \
	(((const uint32_t*) (a))[1] == 0) && \
	(((const uint32_t*) (a))[2] == htonl (0xFFFF)))

typedef uint16_t in_port_t;
typedef uint32_t in_addr_t;

struct in_addr {
	in_addr_t s_addr;
};

struct in6_addr {
	union {
		uint8_t __u6_addr8[16];
		uint16_t __u6_addr16[8];
		uint32_t __u6_addr32[4];
	} __in6_u;
};

#define s6_addr __in6_u.__u6_addr8
#define s6_addr16 __in6_u.__u6_addr16
#define s6_addr32 __in6_u.__u6_addr32

struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	unsigned char sin_zero[
		sizeof(struct sockaddr) -
		sizeof(sa_family_t) -
		sizeof(in_port_t) -
		sizeof(struct in_addr)];
};

struct sockaddr_in6 {
	sa_family_t sin6_family;
	in_port_t sin6_port;
	uint32_t sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t sin6_scope_id;
};

extern struct in6_addr in6addr_loopback;
extern struct in6_addr in6addr_any;

#define INET_ADDRSTRLEN 16
#define INET6_ADDRSTRLEN 46

uint16_t htons(uint16_t __host_short);
uint32_t htonl(uint32_t __host_long);

uint16_t ntohs(uint16_t __net_short);
uint32_t ntohl(uint32_t __net_long);

__end_decls

#endif
