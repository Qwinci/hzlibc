#include "fcntl.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int open(const char* path, int flags, ...) {
	mode_t mode = 0;
	if ((flags & O_CREAT) || (flags & O_TMPFILE)) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	int fd;
	if (auto err = sys_openat(AT_FDCWD, path, flags, mode, &fd)) {
		errno = err;
		return -1;
	}
	return fd;
}

EXPORT int openat(int dir_fd, const char* path, int flags, ...) {
	mode_t mode = 0;
	if ((flags & O_CREAT) || (flags & O_TMPFILE)) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}

	int fd;
	if (auto err = sys_openat(dir_fd, path, flags, mode, &fd)) {
		errno = err;
		return -1;
	}
	return fd;
}

EXPORT int fcntl(int fd, int cmd, ...) {
	va_list args;
	va_start(args, cmd);
	int ret;
	if (auto err = sys_fcntl(fd, cmd, args, &ret)) {
		errno = err;
		va_end(args);
		return -1;
	}
	va_end(args);
	return ret;
}

EXPORT int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
	if (auto err = sys_fadvise(fd, offset, len, advice)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int posix_fallocate(int fd, off_t offset, off_t len) {
	if (auto err = sys_fallocate(fd, 0, offset, len)) {
		return err;
	}
	return 0;
}

EXPORT int posix_fallocate64(int fd, off64_t offset, off64_t len) {
	if (auto err = sys_fallocate(fd, 0, offset, len)) {
		return err;
	}
	return 0;
}

ALIAS(open, open64);
ALIAS(open, __open64_2);
ALIAS(openat, openat64);
ALIAS(fcntl, fcntl64);
