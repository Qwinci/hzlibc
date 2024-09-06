#ifndef _SYS_SENDFILE_H
#define _SYS_SENDFILE_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

ssize_t sendfile(int __out_fd, int __in_fd, off_t* __offset, size_t __count);
ssize_t sendfile64(int __out_fd, int __in_fd, off64_t* __offset, size_t __count);

__end_decls

#endif
