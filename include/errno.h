#ifndef _ERRNO_H
#define _ERRNO_H

#include <bits/utils.h>

__begin

#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENOEXEC 8
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EBUSY 16
#define EEXIST 17
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENOTTY 25
#define ENOSPC 28
#define EPIPE 32
#define ERANGE 34
#define EDEADLK 35
#define ENAMETOOLONG 36
#define ENOSYS 38
#define EOVERFLOW 75
#define EILSEQ 84
#define EMSGSIZE 90
#define EAFNOSUPPORT 97
#define ECONNRESET 104
#define ENOTCONN 107
#define ETIMEDOUT 110
#define ECONNREFUSED 111
#define EHWPOISON 133

int* __errno_location(void);
#define errno (*__errno_location())

// glibc
extern char* program_invocation_name;
extern char* program_invocation_short_name;

__end

#endif
