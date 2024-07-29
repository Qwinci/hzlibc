#include "sys/eventfd.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int eventfd(unsigned int init_value, int flags) {
	int ret;
	if (auto err = sys_eventfd(init_value, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
