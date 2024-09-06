#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#include <bits/utils.h>

#ifdef __x86_64__
#include <bits/syscall_nums_x86_64.h>
#elif defined(__i386__)
#include <bits/syscall_nums_i386.h>
#else
#error unsupported architecture
#endif

__begin_decls

long syscall(long __num, ...);

__end_decls

#endif
