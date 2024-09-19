#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <bits/utils.h>
#include <sys/types.h>
#include <bits/iov.h>

__begin_decls

ssize_t readv(int __fd, const struct iovec* __iov, int __iov_count);
ssize_t writev(int __fd, const struct iovec* __iov, int __iov_count);

// linux

ssize_t process_vm_readv(
	pid_t __pid,
	const struct iovec* __local_iov,
	unsigned long __local_iov_count,
	const struct iovec* __remote_iov,
	unsigned long __remote_iov_count,
	unsigned long __flags);
ssize_t process_vm_writev(
	pid_t __pid,
	const struct iovec* __local_iov,
	unsigned long __local_iov_count,
	const struct iovec* __remote_iov,
	unsigned long __remote_iov_count,
	unsigned long __flags);

__end_decls

#endif
