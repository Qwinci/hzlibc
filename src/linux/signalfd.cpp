#include "sys/signalfd.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int signalfd(int fd, const sigset_t* mask, int flags) {
	int ret;
	if (auto err = sys_signalfd(fd, mask, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
