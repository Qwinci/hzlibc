#include "sys/prctl.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
	int ret;
	if (auto err = sys_prctl(option, arg2, arg3, arg4, arg5, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
