#ifndef _INTTYPES_H
#define _INTTYPES_H

#include <bits/utils.h>
#include <stdint.h>

__begin_decls

typedef struct {
	intmax_t quot;
	intmax_t rem;
} imaxdiv_t;

uintmax_t strtoumax(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);
intmax_t strtoimax(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);

intmax_t imaxabs(intmax_t __x);
imaxdiv_t imaxdiv(intmax_t __x, intmax_t __y);

#define PRIu8 "u"
#define PRIu16 "u"
#define PRIu32 "u"

#define PRId8 "d"
#define PRIi8 PRId8
#define PRId16 "d"
#define PRIi16 PRId16
#define PRId32 "d"
#define PRIi32 PRId32

#define PRIx8 "x"
#define PRIx16 "x"
#define PRIx32 "x"

#define PRIo8 "o"
#define PRIo16 "o"
#define PRIo32 "o"

#define SCNu8 "hhu"
#define SCNu16 "hu"
#define SCNu32 "u"

#define SCNd8 "hhd"
#define SCNd16 "hd"
#define SCNd32 "d"

#define SCNx8 "hhx"
#define SCNx16 "hx"
#define SCNx32 "x"

#if UINTPTR_MAX == UINT64_MAX

#define PRId64 "ld"
#define PRIi64 PRId64
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIX64 "lX"
#define PRIo64 "lo"
#define PRIdPTR "ld"
#define PRIxPTR "lx"
#define PRIXPTR "lX"
#define PRIoPTR "lo"
#define PRIdMAX "ld"
#define PRIuMAX "lu"
#define PRIxMAX "lx"
#define PRIoMAX "lo"

#define SCNd64 "ld"
#define SCNu64 "lu"
#define SCNx64 "lx"

#else

#define PRId64 "lld"
#define PRIi64 PRId64
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"
#define PRIo64 "llo"
#define PRIdPTR "d"
#define PRIxPTR "x"
#define PRIXPTR "X"
#define PRIoPTR "o"
#define PRIdMAX "lld"
#define PRIuMAX "llu"
#define PRIxMAX "llx"
#define PRIoMAX "llo"

#define SCNd64 "lld"
#define SCNu64 "llu"
#define SCNx64 "llx"

#endif

__end_decls

#endif
