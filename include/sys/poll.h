#ifndef _SYS_POLL_H
#define _SYS_POLL_H

#include <bits/utils.h>
#include <time.h>
#include <signal.h>

__begin_decls

#define POLLIN 1
#define POLLPRI 2
#define POLLOUT 4
#define POLLERR 8
#define POLLHUP 0x10
#define POLLNVAL 0x20
#define POLLRDNORM 0x40
#define POLLRDBAND 0x80
#define POLLWRNORM 0x100
#define POLLWRBAND 0x200
#define POLLMSG 0x400
#define POLLREMOVE 0x1000
#define POLLRDHUP 0x2000

typedef unsigned long nfds_t;

struct pollfd {
	int fd;
	short events;
	short revents;
};

int poll(struct pollfd* __fds, nfds_t __num_fds, int __timeout);

// linux
int ppoll(
	struct pollfd* __fds,
	nfds_t __num_fds,
	const struct timespec* __timeout,
	const sigset_t* __sig_mask);

__end_decls

#endif
