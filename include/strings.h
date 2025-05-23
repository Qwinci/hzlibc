#ifndef _STRINGS_H
#define _STRINGS_H

#include <bits/utils.h>
#include <stddef.h>

__begin_decls

int strcasecmp(const char* __s1, const char* __s2);
int strncasecmp(const char* __s1, const char* __s2, size_t __count);

__attribute__((deprecated("Use memcmp instead"))) int bcmp(const void* __s1, const void* __s2, size_t __size);
__attribute__((deprecated("Use memcpy/memmove instead"))) void bcopy(const void* __src, void* __dest, size_t __size);
__attribute__((deprecated("Use memset instead"))) void bzero(void* __dest, size_t __size);

int ffs(int __value);

__end_decls

#endif
