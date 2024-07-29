#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

ssize_t getrandom(void* __buf, size_t __buf_size, unsigned int __flags);

__end

#endif
