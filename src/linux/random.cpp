#include "sys/random.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT ssize_t getrandom(void* buf, size_t buf_size, unsigned int flags) {
	ssize_t ret;
	if (auto err = sys_getrandom(buf, buf_size, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
