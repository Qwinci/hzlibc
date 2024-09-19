#ifndef _ASSERT_H
#define _ASSERT_H

#include <bits/utils.h>

__begin_decls

__attribute__((noreturn)) void __assert_fail(const char* __expr, const char* __file, unsigned int __line, const char* __func);

__end_decls

#endif

#undef assert
#ifndef NDEBUG
#define assert(expr) ((expr) ? (void) 0 : __assert_fail(#expr, __FILE__, __LINE__, __func__))
#else
#define assert(...) ((void) 0)
#endif

#ifndef __cplusplus
#undef static_assert
#define static_assert _Static_assert
#endif
