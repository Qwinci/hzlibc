#ifndef _SYS_EPOLL_H
#define _SYS_EPOLL_H

#include <bits/utils.h>
#include <stdint.h>

__begin_decls

#define EPOLLIN 1
#define EPOLLPRI 2
#define EPOLLOUT 4
#define EPOLLERR 8
#define EPOLLHUP 0x10
#define EPOLLRDNORM 0x40
#define EPOLLRDBAND 0x80
#define EPOLLWRNORM 0x100
#define EPOLLWRBAND 0x200
#define EPOLLMSG 0x400
#define EPOLLRDHUP 0x2000
#define EPOLLEXCLUSIVE 0x10000000
#define EPOLLWAKEUP 0x20000000
#define EPOLLONESHOT 0x40000000
#define EPOLLET 0x80000000

#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3

#define EPOLL_CLOEXEC 0x80000

typedef union epoll_data {
	void* ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

struct epoll_event {
	uint32_t events;
	epoll_data_t data;
} __attribute__((__packed__));

int epoll_create(int __size);
int epoll_create1(int __flags);
int epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event* __event);
int epoll_wait(int __epfd, struct epoll_event* __events, int __max_events, int __timeout);

__end_decls

#endif
