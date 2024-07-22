#ifndef _LIMITS_H
#define _LIMITS_H

#define SCHAR_MIN (-SCHAR_MAX - 1)
#define SCHAR_MAX __SCHAR_MAX__
#define UCHAR_MIN (-UCHAR_MAX - 1)
#define UCHAR_MAX (SCHAR_MAX * 2 + 1)

#if __CHAR_UNSIGNED__
#define CHAR_MAX UCHAR_MAX
#else
#define CHAR_MAX SCHAR_MAX
#endif

#define SHRT_MIN (-SHRT_MAX - 1)
#define SHRT_MAX __SHRT_MAX__
#define USHRT_MAX (SHRT_MAX * 2U + 1U)

#define INT_MIN (-INT_MAX - 1)
#define INT_MAX __INT_MAX__
#define UINT_MAX (INT_MAX * 2U + 1U)
#define LONG_MIN (-LONG_MAX - 1)
#define LONG_MAX __LONG_MAX__
#define ULONG_MAX (LONG_MAX * 2UL + 1UL)
#define LLONG_MIN (-LLONG_MAX - 1)
#define LLONG_MAX __LONG_LONG_MAX__

// posix
#define PATH_MAX 4096
#define PTHREAD_KEYS_MAX 1024
#define PTHREAD_DESTRUCTOR_ITERATIONS 4

#endif
