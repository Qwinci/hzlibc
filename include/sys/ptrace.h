#ifndef _SYS_PTRACE_H
#define _SYS_PTRACE_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

enum __ptrace_request {
	PTRACE_TRACEME = 0,
	PTRACE_PEEKTEXT = 1,
	PTRACE_PEEKDATA = 2,
	PTRACE_PEEKUSER = 3,
	PTRACE_POKETEXT = 4,
	PTRACE_POKEDATA = 5,
	PTRACE_POKEUSER = 6,
	PTRACE_CONT = 7,
	PTRACE_KILL = 8,
	PTRACE_SINGLESTEP = 9,
	PTRACE_GETREGS = 12,
	PTRACE_SETREGS = 13,
	PTRACE_GETFPREGS = 14,
	PTRACE_SETFPREGS = 15,
	PTRACE_ATTACH = 16,
	PTRACE_DETACH = 17,
	PTRACE_GETFPXREGS = 18,
	PTRACE_SETFPXREGS = 19,
	PTRACE_SYSCALL = 24,
	PTRACE_GET_THREAD_AREA = 25,
	PTRACE_SET_THREAD_AREA = 26,
#ifdef __x86_64__
	PTRACE_ARCH_PRCTL = 30,
#endif
	PTRACE_SYSEMU = 31,
	PTRACE_SYSEMU_SINGLESTEP = 32,
	PTRACE_SINGLEBLOCK = 33,
	PTRACE_SETOPTIONS = 0x4200,
	PTRACE_GETEVENTMSG = 0x4201,
	PTRACE_GETSIGINFO = 0x4202,
	PTRACE_SETSIGINFO = 0x4203,
	PTRACE_GETREGSET = 0x4204,
	PTRACE_SETREGSET = 0x4205,
	PTRACE_SEIZE = 0x4206,
	PTRACE_INTERRUPT = 0x4207,
	PTRACE_LISTEN = 0x4208,
	PTRACE_PEEKSIGINFO = 0x4209,
	PTRACE_GETSIGMASK = 0x420A,
	PTRACE_SETSIGMASK = 0x420B,
	PTRACE_SECCOMP_GET_FILTER = 0x420C,
	PTRACE_SECCOMP_GET_METADATA = 0x420D,
	PTRACE_GET_SYSCALL_INFO = 0x420E,
	PTRACE_GET_RSEQ_CONFIGURATION = 0x420F,
	PTRACE_SET_SYSCALL_USER_DISPATCH_CONFIG = 0x4210,
	PTRACE_GET_SYSCALL_USER_DISPATCH_CONFIG = 0x4211
};

long ptrace(int __op, ...);

__end_decls

#endif