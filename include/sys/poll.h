#ifndef _SYS_POLL_H
#define _SYS_POLL_H

#include <bits/utils.h>
#include <time.h>
#include <signal.h>

__begin

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

__end

#endif
