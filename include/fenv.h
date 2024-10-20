#ifndef _FENV_H
#define _FENV_H

#include <bits/utils.h>

__begin_decls

#if defined(__x86_64__) || defined(__i386__)

#define FE_TONEAREST 0
#define FE_DOWNWARD 0x400
#define FE_UPWARD 0x800
#define FE_TOWARDZERO 0xC00

#define FE_INVALID 1
#define __FE_DENORM 2
#define FE_DIVBYZERO 4
#define FE_OVERFLOW 8
#define FE_UNDERFLOW 16
#define FE_INEXACT 32

#define FE_ALL_EXCEPT (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)

#elif defined(__aarch64__)

#define FE_TONEAREST 0
#define FE_UPWARD 0x400000
#define FE_DOWNWARD 0x800000
#define FE_TOWARDZERO 0xC00000

#define FE_INVALID 1
#define FE_DIVBYZERO 2
#define FE_OVERFLOW 4
#define FE_UNDERFLOW 8
#define FE_INEXACT 16

#define FE_ALL_EXCEPT (FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT)

#else

#error missing architecture specific code

#endif

int fesetround(int __round);
int fegetround(void);

int feclearexcept(int __excepts);
int fetestexcept(int __excepts);
int feraiseexcept(int __excepts);

__end_decls

#endif
