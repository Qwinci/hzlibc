#include "sys/uio.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT ssize_t process_vm_readv(
	pid_t pid,
	const struct iovec* local_iov,
	unsigned long local_iov_count,
	const struct iovec* remote_iov,
	unsigned long remote_iov_count,
	unsigned long flags) {
	ssize_t ret;
	if (auto err = sys_process_vm_readv(
		pid,
		local_iov,
		local_iov_count,
		remote_iov,
		remote_iov_count,
		flags,
		&ret
		)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t process_vm_writev(
	pid_t pid,
	const struct iovec* local_iov,
	unsigned long local_iov_count,
	const struct iovec* remote_iov,
	unsigned long remote_iov_count,
	unsigned long flags) {
	ssize_t ret;
	if (auto err = sys_process_vm_writev(
		pid,
		local_iov,
		local_iov_count,
		remote_iov,
		remote_iov_count,
		flags,
		&ret
	)) {
		errno = err;
		return -1;
	}
	return ret;
}
