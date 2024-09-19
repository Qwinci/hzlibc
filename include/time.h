#ifndef _TIME_H
#define _TIME_H

#include <bits/utils.h>
#include <bits/time_t.h>
#include <stddef.h>
#include <stdint.h>
#include <locale.h>

__begin_decls

#ifndef _SYS_TIMES_H
typedef long clock_t;
#endif

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

#define TIME_UTC 1

time_t time(time_t* __arg);
clock_t clock(void);

struct tm* localtime(const time_t* __time);
struct tm* localtime_r(const time_t* __restrict __time, struct tm* __restrict __buf);
struct tm* gmtime(const time_t* __restrict __time);
struct tm* gmtime_r(const time_t* __restrict __time, struct tm* __restrict __buf);
time_t mktime(struct tm* __arg);

size_t strftime(char* __restrict __str, size_t __count, const char* __restrict __fmt, const struct tm* __time);
__attribute__((deprecated)) char* asctime(const struct tm* __time);
__attribute__((deprecated)) char* ctime(const time_t* __time);

double difftime(time_t __b, time_t __a);
int timespec_get(struct timespec* __ts, int __base);

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

char* strptime(const char* __restrict __str, const char* __restrict __fmt, struct tm* __restrict __time);
char* asctime_r(const struct tm* __time, char* __buffer);
char* ctime_r(const time_t* __time, char* __buffer);

int clock_gettime(clockid_t __id, struct timespec* __tp);
int clock_settime(clockid_t __id, const struct timespec* __tp);
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

__end_decls

#endif
