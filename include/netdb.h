#ifndef _NETDB_H
#define _NETDB_H

#include <bits/utils.h>
#include <sys/socket.h>

__begin_decls

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

#define AI_PASSIVE 1
#define AI_CANONNAME 2
#define AI_NUMERICHOST 4
#define AI_V4MAPPED 8
#define AI_ALL 0x10
#define AI_ADDRCONFIG 0x20
#define AI_IDN 0x40
#define AI_CANONIDN 0x80
#define AI_NUMERICSERV 0x400

struct addrinfo {
	int ai_flags;
	int ai_family;
	int ai_socktype;
	int ai_protocol;
	socklen_t ai_addrlen;
	struct sockaddr* ai_addr;
	char* ai_canonname;
	struct addrinfo* ai_next;
};

struct hostent {
	char* h_name;
	char** h_aliases;
	int h_addrtype;
	int h_length;
	char** h_addr_list;
};

struct servent {
	char* s_name;
	char** s_aliases;
	int s_port;
	char* s_proto;
};

#define h_addr h_addr_list[0]

int* __h_errno_location(void);
#define h_errno (*__h_errno_location())

#define HOST_NOT_FOUND 1
#define TRY_AGAIN 2
#define NO_RECOVERY 3
#define NO_DATA 4

int getnameinfo(
	const struct sockaddr* __restrict __addr,
	socklen_t __addr_len,
	char* __host,
	socklen_t __host_len,
	char* __serv,
	socklen_t __serv_len,
	int __flags);

int getaddrinfo(
	const char* __restrict __node_name,
	const char* __restrict __service_name,
	const struct addrinfo* __restrict __hints,
	struct addrinfo** __restrict __res);
void freeaddrinfo(struct addrinfo* __ai);

__attribute__((deprecated("Use getaddrinfo instead")))
struct hostent* gethostbyname(const char* __name);
struct hostent* gethostbyaddr(const void* __addr, socklen_t __len, int __type);

struct servent* getservbyname(const char* __name, const char* __proto);

const char* gai_strerror(int __err_code);

__end_decls

#endif
