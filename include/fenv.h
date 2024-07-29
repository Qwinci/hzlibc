#ifndef _FENV_H
#define _FENV_H

#include <bits/utils.h>

__begin

#if defined(__x86_64__) || defined(__i386__)

#define FE_TONEAREST 0
#define FE_DOWNWARD 0x400
#define FE_UPWARD 0x800
#define FE_TOWARDZERO 0xC00

#else

#error missing architecture specific code

#endif

int fesetround(int __round);
int fegetround(void);
int feclearexcept(int __excepts);
int fetestexcept(int __excepts);

__end

#endif
