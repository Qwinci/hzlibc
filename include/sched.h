#ifndef _SCHED_H
#define _SCHED_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3
#define SCHED_ISO 4
#define SCHED_IDLE 4
#define SCHED_DEADLINE 6
#define SCHED_RESET_ON_FORK 0x40000000

int sched_yield(void);

// glibc
int sched_getcpu(void);

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

typedef struct {
	__cpu_mask __bits[CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

int sched_getaffinity(pid_t __pid, size_t __cpu_set_size, cpu_set_t* __mask);

__end

#endif
