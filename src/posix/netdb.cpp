#include "netdb.h"
#include "utils.hpp"

EXPORT int getnameinfo(
	const struct sockaddr* __restrict addr,
	socklen_t addr_len,
	char* host,
	socklen_t host_len,
	char* serv,
	socklen_t serv_len,
	int flags) {

}
