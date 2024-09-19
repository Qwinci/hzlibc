#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <bits/utils.h>
#include <time.h>
#include <sys/types.h>

__begin_decls

#define ITIMER_REAL 0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF 2

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

struct itimerspec {
	struct timespec it_interval;
	struct timespec it_value;
};

int gettimeofday(struct timeval* __restrict __tv, struct timezone* __restrict __tz);

int setitimer(
	int __which,
	const struct itimerval* __restrict __new_value,
	struct itimerval* __restrict __old_value);

int utimes(const char* __path, const struct timeval __times[2]);

// bsd

#define TIMEVAL_TO_TIMESPEC(tv, ts) { \
	(ts)->tv_sec = (tv)->tv_sec; \
	(ts)->tv_nsec = (tv)->tv_usec * 1000; \
}
#define TIMESPEC_TO_TIMEVAL(tv, ts) { \
	(tv)->tv_sec = (ts)->tv_sec; \
	(tv)->tv_usec = (ts)->tv_nsec / 1000; \
}

#define timerisset(time) ((int) ((time)->tv_sec || (time)->tv_usec))
#define timerclear(time) ((time)->tv_sec = (time)->tv_usec = 0)
#define timercmp(a, b, cmp_op) \
	(((a)->tv_sec == (b)->tv_sec) ? \
	((a)->tv_usec cmp_op (b)->tv_usec) : \
	((a)->tv_sec cmp_op (b)->tv_sec))
#define timeradd(a, b, res) do { \
	(res)->tv_sec = (a)->tv_sec + (b)->tv_sec; \
	(res)->tv_usec = (a)->tv_usec + (b)->tv_usec; \
	if ((res)->tv_usec >= 1000000) { \
		++(res)->tv_sec; \
		(res)->tv_usec -= 1000000; \
    } \
} while (0)
#define timersub(a, b, res) do { \
	(res)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
	(res)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
	if ((res)->tv_usec < 0) { \
		--(res)->tv_sec; \
		(res)->tv_usec += 1000000; \
    } \
} while (0)

int settimeofday(const struct timeval* __time, const struct timezone* __zone);

__end_decls

#endif
