#include "time.h"
#include "sys/time.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "fcntl.h"

#if UINTPTR_MAX == UINT32_MAX
#include "limits.h"
#endif

EXPORT int clock_gettime(clockid_t id, timespec* tp) {
	if (auto err = sys_clock_gettime(id, tp)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int clock_getres(clockid_t id, struct timespec* res) {
	if (auto err = sys_clock_getres(id, res)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int clock_nanosleep(clockid_t id, int flags, const timespec* req, timespec* rem) {
#if UINTPTR_MAX == UINT64_MAX
	auto* req_ptr = reinterpret_cast<const timespec64*>(req);
	auto* rem_ptr = reinterpret_cast<timespec64*>(rem);
	if (auto err = sys_clock_nanosleep(id, flags, req_ptr, rem_ptr)) {
		errno = err;
		return -1;
	}
#else
	timespec64 req_spec {
		.tv_sec = req->tv_sec,
		.tv_nsec = req->tv_nsec
	};
	timespec64 rem_spec {};
	if (rem) {
		rem_spec.tv_sec = rem->tv_sec;
		rem_spec.tv_nsec = rem->tv_nsec;
	}
	if (auto err = sys_clock_nanosleep(id, flags, &req_spec, rem ? &rem_spec : nullptr)) {
		errno = err;
		return -1;
	}

	if (rem) {
		if (rem_spec.tv_sec > LONG_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		rem->tv_sec = static_cast<time_t>(rem_spec.tv_sec);
		rem->tv_nsec = rem_spec.tv_nsec;
	}

#endif
	return 0;
}

EXPORT int nanosleep(const timespec* duration, timespec* rem) {
	return clock_nanosleep(CLOCK_REALTIME, 0, duration, rem);
}

EXPORT void tzset() {
	println("tzset is not implemented");
}

EXPORT size_t strftime_l(
	char* __restrict str,
	size_t count,
	const char* __restrict fmt,
	const struct tm* time,
	locale_t locale) {
	println("strftime_l ignores locale");
	return strftime(str, count, fmt, time);
}

EXPORT char* tzname[2] {};
EXPORT long timezone = 0;
EXPORT int daylight = 0;

EXPORT int gettimeofday(timeval* __restrict tv, struct timezone* __restrict) {
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

EXPORT int setitimer(
	int which,
	const itimerval* __restrict new_value,
	itimerval* __restrict old_value) {
	__ensure(!"setitimer is not implemented");
}

EXPORT int utimes(const char* path, const struct timeval times[2]) {
	timespec64 times64[2] {};
	if (times) {
		times64[0].tv_sec = times[0].tv_sec;
		times64[0].tv_nsec = times[0].tv_usec * 1000;
		times64[1].tv_sec = times[1].tv_sec;
		times64[1].tv_nsec = times[1].tv_usec * 1000;
		if (auto err = sys_utimensat(AT_FDCWD, path, times64, 0)) {
			errno = err;
			return -1;
		}
	}
	else {
		if (auto err = sys_utimensat(AT_FDCWD, path, nullptr, 0)) {
			errno = err;
			return -1;
		}
	}
	return 0;
}

ALIAS(strftime_l, __strftime_l);
ALIAS(timezone, __timezone);
