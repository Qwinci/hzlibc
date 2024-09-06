#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H

#include <bits/utils.h>
#include <sys/time.h>

__begin_decls

typedef long fd_mask;

#define NFDBITS (8 * (int) (sizeof(fd_mask)))
#define FD_SETSIZE 1024

typedef struct {
	fd_mask fds_bits[FD_SETSIZE / NFDBITS];
} fd_set;

int select(
	int __num_fds,
	fd_set* __restrict __read_fds,
	fd_set* __restrict __write_fds,
	fd_set* __restrict __except_fds,
	struct timeval* __restrict __timeout);

__end_decls

#endif
