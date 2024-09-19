#include "sys/time.h"
#include "utils.hpp"
#include "errno.h"
#include "time.h"

EXPORT int settimeofday(const timeval* time, const struct timezone*) {
	if (!time) {
		return 0;
	}
	if (time->tv_usec >= 1000000) {
		errno = EINVAL;
		return -1;
	}
	timespec spec {};
	TIMEVAL_TO_TIMESPEC(time, &spec);
	return clock_settime(CLOCK_REALTIME, &spec);
}
