#ifndef _SYS_PRCTL_H
#define _SYS_PRCTL_H

#include <bits/utils.h>

__begin_decls

#define PR_GET_DUMPABLE 3

int prctl(int __option, unsigned long __arg2, unsigned long __arg3, unsigned long __arg4, unsigned long __arg5);

__end_decls

#endif
