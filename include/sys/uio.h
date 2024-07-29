#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <bits/utils.h>
#include <sys/types.h>
#include <bits/iov.h>

__begin

ssize_t readv(int __fd, const struct iovec* __iov, int __iov_count);
ssize_t writev(int __fd, const struct iovec* __iov, int __iov_count);

__end

#endif
