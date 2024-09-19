#ifndef _SYS_XATTR_H
#define _SYS_XATTR_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

#define XATTR_CREATE 1
#define XATTR_REPLACE 2

ssize_t getxattr(const char* __path, const char* __name, void* __value, size_t __size);
ssize_t lgetxattr(const char* __path, const char* __name, void* __value, size_t __size);
ssize_t fgetxattr(int __fd, const char* __name, void* __value, size_t __size);
int setxattr(const char* __path, const char* __name, const void* __value, size_t __size, int __flags);
int lsetxattr(const char* __path, const char* __name, const void* __value, size_t __size, int __flags);
int fsetxattr(int __fd, const char* __name, const void* __value, size_t __size, int __flags);
int removexattr(const char* __path, const char* __name);
int lremovexattr(const char* __path, const char* __name);
int fremovexattr(int __fd, const char* __name);
ssize_t listxattr(const char* __path, char* __list, size_t __size);
ssize_t llistxattr(const char* __path, char* __list, size_t __size);
ssize_t flistxattr(int __fd, char* __list, size_t __size);

__end_decls

#endif
