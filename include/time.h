#ifndef _TIME_H
#define _TIME_H

#include <bits/utils.h>
#include <stddef.h>
#include <stdint.h>
#include <locale.h>

__begin

typedef long time_t;
typedef long clock_t;
typedef int clockid_t;

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	long tm_gmtoff;
	const char* tm_zone;
};

#define CLOCKS_PER_SEC 1000000

time_t time(time_t* __arg);
clock_t clock(void);

struct tm* localtime(const time_t* __time);
struct tm* localtime_r(const time_t* __restrict __time, struct tm* __restrict __buf);
struct tm* gmtime_r(const time_t* __restrict __time, struct tm* __restrict __buf);
time_t mktime(struct tm* __arg);
size_t strftime(char* __restrict __str, size_t __count, const char* __restrict __fmt, const struct tm* __time);

// posix

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
#define CLOCK_MONOTONIC_RAW 4
#define CLOCK_REALTIME_COARSE 5
#define CLOCK_MONOTONIC_COARSE 6
#define CLOCK_BOOTTIME 7
#define CLOCK_REALTIME_ALARM 8
#define CLOCK_BOOTTIME_ALARM 9

#define TIMER_ABSTIME 1

int clock_gettime(clockid_t __id, struct timespec* __tp);
int clock_getres(clockid_t __id, struct timespec* __res);
int clock_nanosleep(clockid_t __id, int __flags, const struct timespec* __req, struct timespec* __rem);
int nanosleep(const struct timespec* __duration, struct timespec* __rem);

void tzset(void);
time_t timegm(struct tm* __arg);

size_t strftime_l(
	char* __restrict __str,
	size_t __count,
	const char* __restrict __fmt,
	const struct tm* __time,
	locale_t __locale);

extern char* tzname[2];
extern long timezone;
extern int daylight;

__end

#endif
