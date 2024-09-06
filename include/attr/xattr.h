#ifndef _ATTR_XATTR_H
#define _ATTR_XATTR_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

ssize_t listxattr(const char* __path, char* __list, size_t __size);

__end_decls

#endif
