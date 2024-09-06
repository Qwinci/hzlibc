#ifndef _SYS_EPOLL_H
#define _SYS_EPOLL_H

#include <bits/utils.h>

__begin_decls

int epoll_create(int __size);
int epoll_create1(int __flags);

__end_decls

#endif
