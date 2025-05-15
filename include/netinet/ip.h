#ifndef _NETINET_IP_H
#define _NETINET_IP_H

#include <bits/utils.h>
#include <sys/types.h>
#include <netinet/in.h>

struct ip {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	unsigned int ip_hl : 4;
	unsigned int ip_v : 4;
#else
	unsigned int ip_v : 4;
	unsigned int ip_hl : 4;
#endif
	uint8_t ip_tos;
	unsigned short ip_len;
	unsigned short ip_id;
	unsigned short ip_off;
	uint8_t ip_ttl;
	uint8_t ip_p;
	unsigned short ip_sum;
	struct in_addr ip_src;
	struct in_addr ip_dst;
};

#define	IP_RF 0x8000
#define	IP_DF 0x4000
#define	IP_MF 0x2000
#define	IP_OFFMASK 0x1FFF

#endif
