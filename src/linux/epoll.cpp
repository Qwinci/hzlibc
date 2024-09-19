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

EXPORT int epoll_ctl(int epfd, int op, int fd, epoll_event* event) {
	if (auto err = sys_epoll_ctl(epfd, op, fd, event)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int epoll_wait(int epfd, struct epoll_event* events, int max_events, int timeout) {
	int ret;
	if (auto err = sys_epoll_wait(epfd, events, max_events, timeout, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
