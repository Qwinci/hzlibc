#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <bits/utils.h>
#include <sys/socket.h>

__begin_decls

#if defined(_UAPI_LINUX_IN6_H) || defined(_UAPI_IPV6_H) || defined(_LINUX_IN6_H) || defined(_IPV6_H)
#define __KERNEL_IPV6 1
#else
#define __KERNEL_IPV6 0
#endif

#define IPPROTO_IP 0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define IPPROTO_IPV6 41

#if !__KERNEL_IPV6
#define IPPROTO_ICMPV6 58
#endif

#define IP_TOS 1
#define IP_TTL 2
#define IP_MULTICAST_IF 32
#define IP_MULTICAST_TTL 33
#define IP_MULTICAST_LOOP 34
#define IP_ADD_MEMBERSHIP 35
#define IP_DROP_MEMBERSHIP 36
#define IP_UNBLOCK_SOURCE 37
#define IP_BLOCK_SOURCE 38
#define IP_ADD_SOURCE_MEMBERSHIP 39
#define IP_DROP_SOURCE_MEMBERSHIP 40

#define MCAST_JOIN_GROUP 42
#define MCAST_BLOCK_SOURCE 43
#define MCAST_UNBLOCK_SOURCE 44
#define MCAST_LEAVE_GROUP 45
#define MCAST_JOIN_SOURCE_GROUP 46
#define MCAST_LEAVE_SOURCE_GROUP 47

#define IPV6_UNICAST_HOPS 16
#define IPV6_MULTICAST_IF 17
#define IPV6_MULTICAST_HOPS 18
#define IPV6_MULTICAST_LOOP 19
#define IPV6_JOIN_GROUP 20
#define IPV6_LEAVE_GROUP 0x21
#define IPV6_V6ONLY 26

#if !__KERNEL_IPV6
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
#endif

#define IPV6_TCLASS 67

#define INADDR_ANY ((in_addr_t) 0)
#define INADDR_LOOPBACK ((in_addr_t) 0x7F000001)
#define INADDR_BROADCAST ((in_addr_t) 0xFFFFFFFF)
#define INADDR_NONE ((in_addr_t) 0xFFFFFFFF)

#define IN6ADDR_LOOPBACK_INIT {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}}}
#define IN6ADDR_ANY_INIT {{{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}}

#define __ARE_4BYTE_EQUAL(a, b) ((a)[0] == (b)[0] && (a)[1] == (b)[1] && (a)[2] == (b)[2] && (a)[3] == (b)[3])
#define IN6_ARE_ADDR_EQUAL(a, b) __ARE_4BYTE_EQUAL((const uint32_t*) (a), (const uint32_t*) (b))
#define IN6_IS_ADDR_UNSPECIFIED(a) \
	(((const uint32_t*) (a))[0] == 0 && \
	((const uint32_t*) (a))[1] == 0 && \
	((const uint32_t*) (a))[2] == 0 && \
	((const uint32_t*) (a))[3] == 0)
#define IN6_IS_ADDR_LOOPBACK(a) \
	(((const uint32_t*) (a))[0] == 0 && \
	((const uint32_t*) (a))[1] == 0 && \
	((const uint32_t*) (a))[2] == 0 && \
	((const uint32_t*) (a))[3] == htonl (1))
#define IN6_IS_ADDR_V4MAPPED(a) \
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

struct group_source_req {
	uint32_t gsr_interface;
	struct sockaddr_storage gsr_group;
	struct sockaddr_storage gsr_source;
};

struct ip_mreq_source {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
	struct in_addr imr_sourceaddr;
};

struct ip_mreq {
	struct in_addr imr_multiaddr;
	struct in_addr imr_interface;
};

#if !__KERNEL_IPV6

struct ipv6_mreq {
	struct in6_addr ipv6mr_multiaddr;
	unsigned int ipv6mr_interface;
};

#endif

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
