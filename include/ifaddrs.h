#ifndef _IFADDRS_H
#define _IFADDRS_H

#include <bits/utils.h>

__begin_decls

struct ifaddrs {
	struct ifaddrs* ifa_next;
	char* ifa_name;
	unsigned int ifa_flags;
	struct sockaddr* ifa_addr;
	struct sockaddr* ifa_netmask;
	union {
		struct sockaddr* ifu_broadaddr;
		struct sockaddr* ifu_dstaddr;
	} ifa_ifu;
	void* ifa_data;
};

#define ifa_broadcast ifa_ifu.ifu_broadaddr
#define ifa_dstaddr ifa_ifu.ifu_dstaddr

int getifaddrs(struct ifaddrs** __ifap);
void freeifaddrs(struct ifaddrs* __ifa);

__end_decls

#endif
