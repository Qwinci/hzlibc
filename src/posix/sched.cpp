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
