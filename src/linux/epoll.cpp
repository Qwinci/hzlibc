#include "sys/epoll.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int epoll_create(int) {
	return epoll_create1(0);
}

EXPORT int epoll_create1(int flags) {
	int ret;
	if (auto err = sys_epoll_create1(flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
