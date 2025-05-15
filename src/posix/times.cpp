#include "sys/times.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT clock_t times(struct tms* tms) {
	clock_t res;
	if (auto err = sys_times(tms, &res)) {
		errno = err;
		return -1;
	}
	return res;
}
