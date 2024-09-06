#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <bits/utils.h>
#include <sys/time.h>
#include <stdint.h>

__begin_decls

struct rusage {
	struct timeval ru_utime;
	struct timeval ru_stime;
	long ru_maxrss;
	long ru_ixrss;
	long ru_idrss;
	long ru_isrss;
	long ru_minflt;
	long ru_majflt;
	long ru_nswap;
	long ru_inblock;
	long ru_oublock;
	long ru_msgsnd;
	long ru_msgrcv;
	long ru_nsignals;
	long ru_nvcsw;
	long ru_nivcsw;
};

typedef unsigned long rlim_t;
typedef uint64_t rlim64_t;

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

struct rlimit64 {
	rlim64_t rlim_cur;
	rlim64_t rlim_max;
};

#define RLIMIT_CPU 0
#define RLIMIT_FSIZE 1
#define RLIMIT_DATA 2
#define RLIMIT_STACK 3
#define RLIMIT_CORE 4
#define RLIMIT_RSS 5
#define RLIMIT_NPROC 6
#define RLIMIT_NOFILE 7
#define RLIMIT_MEMLOCK 8
#define RLIMIT_AS 9
#define RLIMIT_LOCKS 10
#define RLIMIT_SIGPENDING 11
#define RLIMIT_MSGQUEUE 12
#define RLIMIT_NICE 13
#define RLIMIT_RTPRIO 14
#define RLIMIT_RTTIME 15
#define RLIMIT_NLIMITS 16

#define RLIM_INFINITY ((rlim_t) ~0UL)
#define RLIM64_INFINITY ((rlim_t) ~0ULL)

int setpriority(int __which, int __who, int __prio);

int getrlimit(int __resource, struct rlimit* __rlim);
int getrlimit64(int __resource, struct rlimit64* __rlim);
int setrlimit(int __resource, const struct rlimit* __rlim);
int setrlimit64(int __resource, const struct rlimit64* __rlim);

int getrusage(int __who, struct rusage* __usage);

__end_decls

#endif
