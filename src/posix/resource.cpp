#include "sys/resource.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int setpriority(int which, int who, int prio) {
	if (auto err = sys_setpriority(which, who, prio)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getrlimit(int resource, rlimit* rlim) {
#if UINTPTR_MAX == UINT64_MAX
	if (auto err = sys_getrlimit(resource, reinterpret_cast<rlimit64*>(rlim))) {
		errno = err;
		return -1;
	}
#else
	rlimit64 limit {};
	if (auto err = sys_getrlimit(resource, &limit)) {
		errno = err;
		return -1;
	}
	if (limit.rlim_cur > UINTPTR_MAX) {
		rlim->rlim_cur = RLIM_INFINITY;
	}
	else {
		rlim->rlim_cur = static_cast<rlim_t>(limit.rlim_cur);
	}
	if (limit.rlim_max > UINTPTR_MAX) {
		rlim->rlim_max = RLIM_INFINITY;
	}
	else {
		rlim->rlim_max = static_cast<rlim_t>(limit.rlim_max);
	}
#endif
	return 0;
}

EXPORT int getrlimit64(int resource, rlimit64* rlim) {
	if (auto err = sys_getrlimit(resource, reinterpret_cast<rlimit64*>(rlim))) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int setrlimit(int resource, const rlimit* rlim) {
#if UINTPTR_MAX == UINT64_MAX
	if (auto err = sys_setrlimit(resource, reinterpret_cast<const rlimit64*>(rlim))) {
		errno = err;
		return -1;
	}
#else
	rlimit64 limit {
		.rlim_cur = rlim->rlim_cur,
		.rlim_max = rlim->rlim_max
	};
	if (rlim->rlim_cur == RLIM_INFINITY) {
		limit.rlim_cur = ~0ULL;
	}
	if (rlim->rlim_max == RLIM_INFINITY) {
		limit.rlim_max = ~0ULL;
	}
	if (auto err = sys_setrlimit(resource, &limit)) {
		errno = err;
		return -1;
	}
#endif
	return 0;
}

EXPORT int setrlimit64(int resource, const rlimit64* rlim) {
	if (auto err = sys_setrlimit(resource, rlim)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getrusage(int who, rusage* usage) {
	if (auto err = sys_getrusage(who, usage)) {
		errno = err;
		return -1;
	}
	return 0;
}
