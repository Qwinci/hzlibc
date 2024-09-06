#ifndef _SYS_XATTR_H
#define _SYS_XATTR_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

ssize_t getxattr(const char* __path, const char* __name, void* __value, size_t __size);
ssize_t fgetxattr(int __fd, const char* __name, void* __value, size_t __size);
int setxattr(const char* __path, const char* __name, const void* __value, size_t __size, int __flags);
int fsetxattr(int __fd, const char* __name, const void* __value, size_t __size, int __flags);
int removexattr(const char* __path, const char* __name);
int fremovexattr(int __fd, const char* __name);

__end_decls

#endif
