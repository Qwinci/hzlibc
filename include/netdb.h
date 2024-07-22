#ifndef _NETDB_H
#define _NETDB_H

#include <bits/utils.h>
#include <sys/socket.h>

__begin

#define EAI_BADFLAGS -1
#define EAI_NONAME -2
#define EAI_AGAIN -3
#define EAI_FAIL -4
#define EAI_NODATA -5
#define EAI_FAMILY -6
#define EAI_SOCKTYPE -7
#define EAI_SERVICE -8
#define EAI_ADDRFAMILY -9
#define EAI_MEMORY -10
#define EAI_SYSTEM -11
#define EAI_OVERFLOW -12
#define EAI_INPROGRESS -100
#define EAI_CANCELED -101
#define EAI_NOTCANCELED -102
#define EAI_ALLDONE -103
#define EAI_INTR -104
#define EAI_IDN_ENCODE -105

#define NI_MAXHOST 1025
#define NI_MAXSERV 32

#define NI_NUMERICHOST 1
#define NI_NUMERICSERV 2
#define NI_NOFQDN 4
#define NI_NAMEREQD 8
#define NI_DGRAM 16
#define NI_IDN 32

int getnameinfo(
	const struct sockaddr* __restrict __addr,
	socklen_t __addr_len,
	char* __host,
	socklen_t __host_len,
	char* __serv,
	socklen_t __serv_len,
	int __flags);

__end

#endif
