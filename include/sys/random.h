#ifndef _SYS_RANDOM_H
#define _SYS_RANDOM_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

#define GRND_NONBLOCK 1
#define GRND_RANDOM 2
#define GRND_INSECURE 4

ssize_t getrandom(void* __buf, size_t __buf_size, unsigned int __flags);

__end_decls

#endif
