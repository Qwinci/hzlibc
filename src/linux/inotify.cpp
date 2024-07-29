#include "sys/inotify.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int inotify_init() {
	int ret;
	if (auto err = sys_inotify_init1(0, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int inotify_init1(int flags) {
	int ret;
	if (auto err = sys_inotify_init1(flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int inotify_add_watch(int fd, const char* path, uint32_t mask) {
	int ret;
	if (auto err = sys_inotify_add_watch(fd, path, mask, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int inotify_rm_watch(int fd, int wd) {
	if (auto err = sys_inotify_rm_watch(fd, wd)) {
		errno = err;
		return -1;
	}
	return 0;
}
