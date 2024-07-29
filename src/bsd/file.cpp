#include "sys/file.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int flock(int fd, int operation) {
	if (auto err = sys_flock(fd, operation)) {
		errno = err;
		return -1;
	}
	return 0;
}
