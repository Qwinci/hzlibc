#include "sys/uio.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT ssize_t readv(int fd, const struct iovec* iov, int iov_count) {
	ssize_t ret;
	if (auto err = sys_readv(fd, iov, iov_count, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t writev(int fd, const struct iovec* iov, int iov_count) {
	ssize_t ret;
	if (auto err = sys_writev(fd, iov, iov_count, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
