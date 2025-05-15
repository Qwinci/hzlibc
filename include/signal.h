#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <bits/utils.h>
#include <bits/config.h>
#include <bits/sigset_t.h>
#include <time.h>

#if !__HZLIBC_ANSI_ONLY
#include <sys/types.h>
#include <ucontext.h>
#endif

__begin_decls

typedef void (*sighandler_t)(int __sig_num);

#define SIG_DFL ((__sighandler) ((void*) 0))
#define SIG_IGN ((__sighandler) ((void*) 1))

typedef int sig_atomic_t;

#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGABRT 6
#define SIGIOT SIGABRT
#define SIGBUS 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGSTKFLT 16
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20
#define SIGTTIN 21
#define SIGTTOU 22
#define SIGURG 23
#define SIGXCPU 24
#define SIGXFSZ 25
#define SIGVTALRM 26
#define SIGPROF 27
#define SIGWINCH 28
#define SIGIO 29
#define SIGPOLL SIGIO
#define SIGPWR 30
#define SIGSYS 31
#define SIGUNUSED SIGSYS
#define SIGRTMIN 35
#define SIGRTMAX 64

sighandler_t signal(int __sig_num, sighandler_t __handler);
int raise(int __sig);

#if !__HZLIBC_ANSI_ONLY

// posix

#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO 4
#define SA_RESTORER 0x4000000
#define SA_ONSTACK 0x8000000
#define SA_RESTART 0x10000000
#define SA_NODEFER 0x40000000
#define SA_RESETHAND 0x80000000

#define SS_ONSTACK 1
#define SS_DISABLE 2

typedef void (*__sighandler)(int);

#define SIG_ERR ((__sighandler) ((void*) -1))

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SI_ASYNCNL -60
#define SI_DETHREAD -7
#define SI_TKILL -6
#define SI_SIGIO -5
#define SI_ASYNCIO -4
#define SI_MESGQ -3
#define SI_TIMER -2
#define SI_QUEUE -1
#define SI_USER 0
#define SI_KERNEL 0x80

#define TRAP_BRKPT 1
#define TRAP_TRACE 2
#define TRAP_BRANCH 3
#define TRAP_HWBKPT 4
#define TRAP_UNK 5

#define MINSIGSTKSZ 2048
#define SIGSTKSZ 8192

#define SI_KERNEL 0x80

#define CLD_EXITED 1
#define CLD_KILLED 2
#define CLD_DUMPED 3
#define CLD_TRAPPED 4
#define CLD_STOPPED 5
#define CLD_CONTINUED 6

#define FPE_INTDIV 1
#define FPE_INTOVF 2
#define FPE_FLTDIV 3
#define FPE_FLTOVF 4
#define FPE_FLTUND 5
#define FPE_FLTRES 6
#define FPE_FLTINV 7
#define FPE_FLTSUB 8

#define ILL_ILLOPC 1
#define ILL_ILLOPN 2
#define ILL_ILLADR 3
#define ILL_ILLTRP 4
#define ILL_PRVOPC 5
#define ILL_PRVREG 6
#define ILL_COPROC 7
#define ILL_BADSTK 8
#define ILL_BADIADDR 9

#define BUS_ADRALN 1
#define BUS_ADRERR 2
#define BUS_OBJERR 3
#define BUS_MCEERR_AR 4
#define BUS_MCEERR_AO 5

#define SEGV_MAPERR 1
#define SEGV_ACCERR 2
#define SEGV_BNDERR 3
#define SEGV_PKUERR 4
#define SEGV_ACCADI 5
#define SEGV_ADIDERR 6
#define SEGV_ADIPERR 7
#define SEGV_MTEAERR 8
#define SEGV_MTESERR 9
#define SEGV_CPERR 10

#define POLL_IN 1
#define POLL_OUT 2
#define POLL_MSG 3
#define POLL_ERR 4
#define POLL_PRI 5
#define POLL_HUP 6

#define NSIG 65
#define _NSIG NSIG

typedef union sigval {
	int sival_int;
	void* sival_ptr;
} sigval_t;

typedef struct A {
	int si_signo;
	int si_errno;
	int si_code;
#if UINTPTR_MAX == UINT64_MAX
	int __pad0;
#endif
	union {
		char __pad[128 - 2 * sizeof(int) - sizeof(long)];

		struct {
			int si_timerid;
			int si_overrun;
			sigval_t si_sigval;
		} __timer;

		struct {
			pid_t si_pid;
			uid_t si_uid;
		} __piduid;

		struct {
			pid_t si_pid;
			uid_t si_uid;
			sigval_t si_value;
		} __rt;

		struct {
			pid_t si_pid;
			uid_t si_uid;
			int si_status;
			clock_t si_utime;
			clock_t si_stime;
		} __sigchld;

		struct {
			void* si_addr;
			short si_addr_lsb;
			union {
				struct {
					void* si_lower;
					void* si_upper;
				} __addr_bnd;
				uint32_t si_pkey;
			};
		} __sigfault;

		struct {
			long si_band;
			int si_fd;
		} __sigpoll;

		struct {
			void* si_call_addr;
			int si_syscall;
			unsigned int si_arch;
		} __sigsys;
	} __si_fields;
} siginfo_t;

#define si_pid __si_fields.__piduid.si_pid
#define si_uid __si_fields.__piduid.si_uid
#define si_timerid __si_fields.__timer.si_tid
#define si_overrun __si_fields.__timer.si_overrun
#define si_status __si_fields.__sigchld.si_status
#define si_utime __si_fields.__sigchld.si_utime
#define si_stime __si_fields.__sigchld.si_stime
#define si_value __si_fields.__rt.si_value
#define si_int __si_fields.__rt.si_value.sival_int
#define si_ptr __si_fields.__rt.si_value.sival_ptr
#define si_addr __si_fields.__sigfault.si_addr
#define si_addr_lsb __si_fields.__sigfault.si_addr_lsb
#define si_lower __si_fields.__sigfault.si_lower
#define si_upper __si_fields.__sigfault.si_upper
#define si_pkey __si_fields.__sigfault.si_pkey
#define si_band __si_fields.__sigpoll.si_band
#define si_fd __si_fields.__sigpoll.si_fd
#define si_call_addr __si_fields.__sigsys.si_call_addr
#define si_syscall __si_fields.__sigsys.si_syscall
#define si_arch __si_fields.__sigsys.si_arch

struct sigaction {
	union {
		void (*sa_handler)(int __sig_num);
		void (*sa_sigaction)(int __sig_num, siginfo_t* __info, void* __mcontext);
	};
	sigset_t sa_mask;
	unsigned long sa_flags;
	void (*sa_restorer)(void);
};

int kill(pid_t __pid, int __sig);
int killpg(pid_t __pgrp, int __sig);
int sigprocmask(int __how, const sigset_t* __restrict __set, sigset_t* __restrict __old);
int pthread_sigmask(int __how, const sigset_t* __restrict __set, sigset_t* __restrict __old);
int sigaction(int __sig_num, const struct sigaction* __restrict __action, struct sigaction* __restrict __old);
int sigtimedwait(const sigset_t* __restrict __set, siginfo_t* __restrict __info, const struct timespec* __timeout);
int sigwait(const sigset_t* __restrict __set, int* __restrict __sig);
int sigaltstack(const stack_t* __stack, stack_t* __old_stack);
int sigsuspend(const sigset_t* __set);
int sigpending(sigset_t* __set);

int sigemptyset(sigset_t* __set);
int sigfillset(sigset_t* __set);
int sigismember(const sigset_t* __set, int __sig_num);
int sigaddset(sigset_t* __set, int __sig_num);
int sigdelset(sigset_t* __set, int __sig_num);

#endif

__end_decls

#endif
