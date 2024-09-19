#include "sched.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int sched_getcpu() {
	int ret;
	if (auto err = sys_sched_getcpu(&ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int clone(int (*fn)(void* __arg), void* stack, int flags, void* arg, ...) {
	__ensure(!"clone is not implemented");
}

EXPORT int setns(int fd, int ns_type) {
	if (auto err = sys_setns(fd, ns_type)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int __sched_cpucount(size_t set_size, const cpu_set_t* set) {
	int count = 0;
	for (size_t i = 0; i < set_size / sizeof(__cpu_mask); ++i) {
		auto value = set->__bits[i];
		for (; value; ++count) {
			value &= value - 1;
		}
	}
	return count;
}
