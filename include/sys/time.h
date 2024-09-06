#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <bits/utils.h>
#include <time.h>
#include <sys/types.h>

__begin_decls

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

struct itimerval {
	struct timeval it_interval;
	struct timeval it_value;
};

int gettimeofday(struct timeval* __restrict __tv, struct timezone* __restrict __tz);

int setitimer(
	int __which,
	const struct itimerval* __restrict __new_value,
	struct itimerval* __restrict __old_value);

int utimes(const char* __path, const struct timeval __times[2]);

__end_decls

#endif
