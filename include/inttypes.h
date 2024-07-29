#ifndef _INTTYPES_H
#define _INTTYPES_H

#include <bits/utils.h>
#include <stdint.h>

__begin

uintmax_t strtoumax(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);
intmax_t strtoimax(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);

#define PRIu8 "u"

#if UINTPTR_MAX == UINT64_MAX
#define PRId64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIdPTR "ld"
#define PRIxPTR "lx"
#else
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIdPTR "d"
#define PRIxPTR "x"
#endif

__end

#endif
