#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <bits/utils.h>
#include <sys/types.h>
#include <time.h>

__begin

typedef void (*sighandler_t)(int __sig_num);

sighandler_t signal(int __sig_num, sighandler_t __handler);
int raise(int __sig);

// posix

#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO 4
#define SA_RESTORER 0x4000000
#define SA_ONSTACK 0x8000000
#define SA_RESTART 0x10000000
#define SA_NODEFER 0x40000000
#define SA_RESETHAND 0x80000000

typedef void (*__sighandler)(int);

#define SIG_ERR ((__sighandler) ((void*) -1))
#define SIG_DFL ((__sighandler) ((void*) 0))
#define SIG_IGN ((__sighandler) ((void*) 1))

#define SIG_BLOCK 0
#define SIG_UNLOCK 1
#define SIG_SETMASK 2

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

#define MINSIGSTKSZ 2048
#define SIGSTKSZ 8192

typedef struct {
	unsigned long __value[1024 / (8 * sizeof(unsigned long))];
} sigset_t;

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

struct sigaction {
	union {
		void (*sa_handler)(int __sig_num);
		void (*sa_sigaction)(int __sig_num, siginfo_t* __info, void* __mcontext);
	};
	sigset_t sa_mask;
	unsigned long sa_flags;
	void (*sa_restorer)(void);
};

int sigprocmask(int __how, const sigset_t* __restrict __set, sigset_t* __restrict __old);
int pthread_sigmask(int __how, const sigset_t* __restrict __set, sigset_t* __restrict __old);
int sigaction(int __sig_num, const struct sigaction* __restrict __action, struct sigaction* __restrict __old);
int sigemptyset(sigset_t* __set);
int sigismember(const sigset_t* __set, int __sig_num);
int sigaddset(sigset_t* __set, int __sig_num);

__end

#endif
