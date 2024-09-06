#ifndef _UTIME_H
#define _UTIME_H

#include <bits/utils.h>
#include <time.h>

__begin_decls

struct utimbuf {
	time_t actime;
	time_t modtime;
};

int utime(const char* __path, const struct utimbuf* __times);

__end_decls

#endif
