#include "sys/poll.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int poll(struct pollfd* fds, nfds_t num_fds, int timeout) {
	int ret;
	if (auto err = sys_poll(fds, num_fds, timeout, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
