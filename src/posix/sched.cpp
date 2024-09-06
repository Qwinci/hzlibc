#include "sched.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int sched_yield() {
	if (auto err = sys_sched_yield()) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int sched_get_priority_min(int policy) {
	int ret;
	if (auto err = sys_sched_get_priority_min(policy, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int sched_get_priority_max(int policy) {
	int ret;
	if (auto err = sys_sched_get_priority_max(policy, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
