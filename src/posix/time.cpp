#include "time.h"
#include "sys/time.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int clock_gettime(clockid_t id, struct timespec* tp) {
	if (auto err = sys_clock_gettime(id, tp)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT void tzset() {
	println("tzset is not implemented");
}

EXPORT char* tzname[2] {};
EXPORT long timezone = 0;
EXPORT int daylight = 0;

EXPORT int gettimeofday(struct timeval* __restrict tv, struct timezone* __restrict) {
	timespec tp {};
	if (auto err = sys_clock_gettime(CLOCK_REALTIME, &tp)) {
		errno = err;
		return -1;
	}
	if (tv) {
		tv->tv_sec = tp.tv_sec;
		tv->tv_usec = tp.tv_nsec / 1000;
	}
	return 0;
}
