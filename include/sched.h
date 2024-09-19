#ifndef _SCHED_H
#define _SCHED_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3
#define SCHED_ISO 4
#define SCHED_IDLE 4
#define SCHED_DEADLINE 6
#define SCHED_RESET_ON_FORK 0x40000000

struct sched_param {
	int sched_priority;
};

int sched_yield(void);
int sched_get_priority_min(int __policy);
int sched_get_priority_max(int __policy);

// glibc
int sched_getcpu(void);
int clone(int (*__fn)(void* __arg), void* __stack, int __flags, void* __arg, ...);
int setns(int __fd, int __ns_type);

// linux

#define CLONE_VM 0x100
#define CLONE_FS 0x200
#define CLONE_FILES 0x400
#define CLONE_SIGHAND 0x800
#define CLONE_PIDFD 0x1000
#define CLONE_PTRACE 0x2000
#define CLONE_VFORK 0x4000
#define CLONE_PARENT 0x8000
#define CLONE_THREAD 0x10000
#define CLONE_NEWNS 0x20000
#define CLONE_SYSVSEM 0x40000
#define CLONE_SETTLS 0x80000
#define CLONE_PARENT_SETTID 0x100000
#define CLONE_CHILD_CLEARTID 0x200000
#define CLONE_DETACHED 0x400000
#define CLONE_UNTRACED 0x800000
#define CLONE_CHILD_SETTID 0x1000000
#define CLONE_NEWCGROUP 0x2000000
#define CLONE_NEWUTS 0x4000000
#define CLONE_NEWIPC 0x8000000
#define CLONE_NEWUSER 0x10000000
#define CLONE_NEWPID 0x20000000
#define CLONE_NEWNET 0x40000000
#define CLONE_IO 0x80000000

typedef unsigned long __cpu_mask;

#define CPU_SETSIZE	1024
#define __NCPUBITS (8 * sizeof(__cpu_mask))
#define __CPUELT(cpu) ((cpu) / __NCPUBITS)
#define __CPUMASK(cpu) ((__cpu_mask) 1 << ((cpu) % __NCPUBITS))
#define CPU_ISSET_S(cpu, set_size, set) ({ \
	size_t __cpu = (cpu); \
	__cpu / 8 < (set_size) ? \
	((set)->__bits[__CPUELT(__cpu)] & __CPUMASK(__cpu)) != 0 \
	: 0; \
})
#define CPU_ISSET(cpu, set) CPU_ISSET_S(cpu, sizeof(cpu_set_t), set)
#define CPU_SET_S(cpu, set_size, set) do { \
	size_t __cpu = (cpu); \
	if (__cpu / 8 < (set_size)) { \
		(set)->__bits[__CPUELT(__cpu)] |= __CPUMASK(__cpu); \
	} \
} while (0)
#define CPU_SET(cpu, set) CPU_SET_S(cpu, sizeof(cpu_set_t), set)
#define CPU_CLR_S(cpu, set_size, set) do { \
	size_t __cpu = (cpu); \
	if (__cpu / 8 < (set_size)) { \
		(set)->__bits[__CPUELT(__cpu)] &= ~__CPUMASK(__cpu); \
	} \
} while (0)
#define CPU_CLR(cpu, set) CPU_CLR_S(cpu, sizeof(cpu_set_t), set)
#define CPU_ZERO_S(set_size, set) do { __builtin_memset((set), 0, (set_size)); } while (0)
#define CPU_ZERO(set) CPU_ZERO_S(sizeof(cpu_set_t), set)
#define CPU_COUNT_S(set_size, set) __sched_cpucount((set_size), (set))
#define CPU_COUNT(set) CPU_COUNT_S(sizeof(cpu_set_t), (set))

#define CPU_ALLOC_SIZE(count) ((((count) + __NCPUBITS - 1) / __NCPUBITS) * sizeof(__cpu_mask))
#define CPU_ALLOC(count) __sched_cpualloc(count)
#define CPU_FREE(set) __sched_cpufree(set)

typedef struct {
	__cpu_mask __bits[CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

int sched_setaffinity(pid_t __pid, size_t __cpu_set_size, const cpu_set_t* __mask);
int sched_getaffinity(pid_t __pid, size_t __cpu_set_size, cpu_set_t* __mask);
int unshare(int __flags);

cpu_set_t* __sched_cpualloc(size_t __count);
void __sched_cpufree(cpu_set_t* __set);
int __sched_cpucount(size_t __set_size, const cpu_set_t* __set);

__end_decls

#endif
