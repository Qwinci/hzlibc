#include "sys/swap.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int swapon(const char* path, int flags) {
	if (auto err = sys_swapon(path, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int swapoff(const char* path) {
	if (auto err = sys_swapoff(path)) {
		errno = err;
		return -1;
	}
	return 0;
}
