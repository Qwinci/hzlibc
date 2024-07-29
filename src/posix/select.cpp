#include "sys/select.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int select(
	int num_fds,
	fd_set* __restrict read_fds,
	fd_set* __restrict write_fds,
	fd_set* __restrict except_fds,
	struct timeval* __restrict timeout) {
	int ret;
	if (auto err = sys_select(num_fds, read_fds, write_fds, except_fds, timeout, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
