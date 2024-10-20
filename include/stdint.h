#ifndef _STDINT_H
#define _STDINT_H

typedef __INT8_TYPE__ int8_t;
typedef __INT16_TYPE__ int16_t;
typedef __INT32_TYPE__ int32_t;
typedef __INT64_TYPE__ int64_t;
typedef __INTPTR_TYPE__ intptr_t;
typedef __INTMAX_TYPE__ intmax_t;

typedef __UINT8_TYPE__ uint8_t;
typedef __UINT16_TYPE__ uint16_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __UINT64_TYPE__ uint64_t;
typedef __UINTPTR_TYPE__ uintptr_t;
typedef __UINTMAX_TYPE__ uintmax_t;

#if __UINTPTR_MAX__ == __UINT64_MAX__

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

#define INT_LEAST8_C(value) value
#define INT_LEAST16_C(value) value
#define INT_LEAST32_C(value) value
#define INT_LEAST64_C(value) value ## L

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

#define UINT_LEAST8_C(value) value
#define UINT_LEAST16_C(value) value
#define UINT_LEAST32_C(value) value ## U
#define UINT_LEAST64_C(value) value ## UL

typedef int8_t int_fast8_t;
typedef int64_t int_fast16_t;
typedef int64_t int_fast32_t;
typedef int64_t int_fast64_t;

#define INT_FAST8_C(value) value
#define INT_FAST16_C(value) value ## L
#define INT_FAST32_C(value) value ## L
#define INT_FAST64_C(value) value ## L

typedef uint8_t uint_fast8_t;
typedef uint64_t uint_fast16_t;
typedef uint64_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

#define UINT_FAST8_C(value) value
#define UINT_FAST16_C(value) value ## UL
#define UINT_FAST32_C(value) value ## UL
#define UINT_FAST64_C(value) value ## UL

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT64_MAX
#define INT_FAST32_MAX INT64_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT64_MAX
#define UINT_FAST32_MAX UINT64_MAX
#define UINT_FAST64_MAX UINT64_MAX

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT64_MIN
#define INT_FAST32_MIN INT64_MIN
#define INT_FAST64_MIN INT64_MIN

#else

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

#define INT_LEAST8_C(value) value
#define INT_LEAST16_C(value) value
#define INT_LEAST32_C(value) value
#define INT_LEAST64_C(value) value ## LL

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

#define UINT_LEAST8_C(value) value
#define UINT_LEAST16_C(value) value
#define UINT_LEAST32_C(value) value ## U
#define UINT_LEAST64_C(value) value ## ULL

typedef int8_t int_fast8_t;
typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

#define INT_FAST8_C(value) value
#define INT_FAST16_C(value) value ## L
#define INT_FAST32_C(value) value ## L
#define INT_FAST64_C(value) value ## LL

typedef uint8_t uint_fast8_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

#define UINT_FAST8_C(value) value
#define UINT_FAST16_C(value) value ## UL
#define UINT_FAST32_C(value) value ## UL
#define UINT_FAST64_C(value) value ## ULL

#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX

#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT32_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX

#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT32_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN

#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT32_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN

#endif

#define INT8_MIN (-INT8_MAX - 1)
#define INT8_MAX __INT8_MAX__
#define INT16_MIN (-INT16_MAX - 1)
#define INT16_MAX __INT16_MAX__
#define INT32_MIN (-INT32_MAX - 1)
#define INT32_MAX __INT32_MAX__
#define INT64_MIN (-INT64_MAX - 1)
#define INT64_MAX __INT64_MAX__
#define INTPTR_MIN (-INTPTR_MAX - 1)
#define INTPTR_MAX __INTPTR_MAX__
#define INTMAX_MIN (-INTMAX_MAX - 1)
#define INTMAX_MAX __INTMAX_MAX__

#define UINT8_MAX __UINT8_MAX__
#define UINT16_MAX __UINT16_MAX__
#define UINT32_MAX __UINT32_MAX__
#define UINT64_MAX __UINT64_MAX__
#define UINTPTR_MAX __UINTPTR_MAX__
#define UINTMAX_MAX __UINTMAX_MAX__

#define PTRDIFF_MIN (-PTRDIFF_MAX - 1)
#define PTRDIFF_MAX __PTRDIFF_MAX__
#define SIZE_MAX __SIZE_MAX__

#define WINT_MIN __WINT_MIN__
#define WINT_MAX __WINT_MAX__
#define WCHAR_MIN __WCHAR_MIN__
#define WCHAR_MAX __WCHAR_MAX__

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX

#define INT8_C(value) value
#define INT16_C(value) value
#define INT32_C(value) value

#define UINT8_C(value) value
#define UINT16_C(value) value
#define UINT32_C(value) value ## U

#if UINTPTR_MAX == UINT64_MAX
#define INT64_C(value) value ## L
#define UINT64_C(value) value ## UL
#define UINTMAX_C(value) value ## UL
#else
#define INT64_C(value) value ## LL
#define UINT64_C(value) value ## ULL
#define UINTMAX_C(value) value ## ULL
#endif

#endif
