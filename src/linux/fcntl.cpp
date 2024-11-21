#include "fcntl.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int fallocate(int fd, int mode, off_t offset, off_t len) {
	if (auto err = sys_fallocate(fd, mode, offset, len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fallocate64(int fd, int mode, off64_t offset, off64_t len) {
	if (auto err = sys_fallocate(fd, mode, offset, len)) {
		errno = err;
		return -1;
	}
	return 0;
}
