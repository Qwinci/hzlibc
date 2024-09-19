#include "sched.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "stdlib.h"

EXPORT int sched_setaffinity(pid_t pid, size_t cpu_set_size, const cpu_set_t* mask) {
	if (auto err = sys_sched_setaffinity(pid, cpu_set_size, mask)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int sched_getaffinity(pid_t pid, size_t cpu_set_size, cpu_set_t* mask) {
	if (auto err = sys_sched_getaffinity(pid, cpu_set_size, mask)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int unshare(int flags) {
	if (auto err = sys_unshare(flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT cpu_set_t* __sched_cpualloc(size_t count) {
	return static_cast<cpu_set_t*>(malloc(CPU_ALLOC_SIZE(count)));
}

EXPORT void __sched_cpufree(cpu_set_t* set) {
	free(set);
}
