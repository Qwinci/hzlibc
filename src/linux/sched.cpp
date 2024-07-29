#include "sched.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int sched_getaffinity(pid_t pid, size_t cpu_set_size, cpu_set_t* mask) {
	if (auto err = sys_sched_getaffinity(pid, cpu_set_size, mask)) {
		errno = err;
		return -1;
	}
	return 0;
}
