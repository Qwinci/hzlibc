#include "sys/socket.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int accept4(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len, int flags) {
	int ret;
	if (auto err = sys_accept4(fd, addr, addr_len, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
