#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <bits/utils.h>
#include <time.h>
#include <sys/types.h>

__begin

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

int gettimeofday(struct timeval* __restrict __tv, struct timezone* __restrict __tz);

__end

#endif
