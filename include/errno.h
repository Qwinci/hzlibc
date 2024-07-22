#ifndef _ERRNO_H
#define _ERRNO_H

#include <bits/utils.h>

__begin

#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EINVAL 22
#define ENOTTY 25
#define ERANGE 34
#define EDEADLK 35
#define ENAMETOOLONG 36
#define ENOSYS 38
#define EOVERFLOW 75
#define EILSEQ 84
#define EHWPOISON 133

int* __errno_location(void);
#define errno (*__errno_location())

// glibc
extern char* program_invocation_name;
extern char* program_invocation_short_name;

__end

#endif
