#ifndef _SYS_SENDFILE_H
#define _SYS_SENDFILE_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

ssize_t sendfile(int __out_fd, int __in_fd, off_t* __offset, size_t __count);

__end

#endif
