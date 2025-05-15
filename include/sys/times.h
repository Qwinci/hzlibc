#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

#include <bits/utils.h>

__begin_decls

#ifndef _TIME_H
typedef long clock_t;
#endif

struct tms {
	clock_t tms_utime;
	clock_t tms_stime;
	clock_t tms_cutime;
	clock_t tms_cstime;
};

clock_t times(struct tms* __tms);

__end_decls

#endif
