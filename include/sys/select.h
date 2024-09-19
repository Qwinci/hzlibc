#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <bits/utils.h>
#include <sys/time.h>
#include <signal.h>

__begin_decls

typedef long fd_mask;

#define NFDBITS (8 * (int) (sizeof(fd_mask)))
#define FD_SETSIZE 1024

#define FD_ZERO(set) __builtin_memset(set, 0, sizeof(fd_set))
#define FD_SET(fd, set) ((void) ((set)->fds_bits[(fd) / NFDBITS] |= 1L << ((fd) % NFDBITS)))
#define FD_CLR(fd, set) ((void) ((set)->fds_bits[(fd) / NFDBITS] &= ~(1L << ((fd) % NFDBITS))))
#define FD_ISSET(fd, set) (((set)->fds_bits[(fd) / NFDBITS] & 1L << ((fd) % NFDBITS)) != 0)

typedef struct {
	fd_mask fds_bits[FD_SETSIZE / NFDBITS];
} fd_set;

int select(
	int __num_fds,
	fd_set* __restrict __read_fds,
	fd_set* __restrict __write_fds,
	fd_set* __restrict __except_fds,
	struct timeval* __restrict __timeout);

int pselect(
	int __num_fds,
	fd_set* __restrict __read_fds,
	fd_set* __restrict __write_fds,
	fd_set* __restrict __except_fds,
	const struct timespec* __restrict __timeout,
	const sigset_t* __sig_mask);

__end_decls

#endif
