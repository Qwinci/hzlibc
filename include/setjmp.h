#ifndef _SIGJMP_H
#define _SIGJMP_H

#include <bits/utils.h>
#include <signal.h>

__begin_decls

#ifdef __x86_64__
typedef long __jmp_buf[8];
#elif defined(__i386__)
typedef long __jmp_buf[6];
#else
#error missing architecture specific code
#endif

struct __jmp_buf_tag {
	__jmp_buf __buf;
	int __saved_mask;
	sigset_t __sigset;
};

typedef struct __jmp_buf_tag jmp_buf[1];

__attribute__((__returns_twice__)) int setjmp(jmp_buf __env);
__attribute__((__noreturn__)) void longjmp(jmp_buf __env, int __value);

// posix
__attribute__((__returns_twice__)) int _setjmp(jmp_buf __env);
__attribute__((__noreturn__)) void _longjmp(jmp_buf __env, int __value);

typedef struct __jmp_buf_tag sigjmp_buf[1];

__attribute__((__returns_twice__)) int sigsetjmp(sigjmp_buf __env, int __save_mask);
__attribute__((__noreturn__)) void siglongjmp(sigjmp_buf __env, int __value);

__end_decls

#endif
