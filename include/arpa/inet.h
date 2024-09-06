#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <bits/utils.h>
#include <sys/socket.h>

__begin_decls

uint16_t htons(uint16_t __host_short);
uint32_t htonl(uint32_t __host_long);

uint16_t ntohs(uint16_t __net_short);
uint32_t ntohl(uint32_t __net_long);

const char* inet_ntop(int __af, const void* __restrict __src, char* __dest, socklen_t __size);
int inet_pton(int __af, const char* __restrict __src, void* __restrict __dest);

__end_decls

#endif
