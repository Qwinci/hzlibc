#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <bits/utils.h>

__begin_decls

typedef union {
#if defined(__x86_64__) || defined(__aarch64__)
	char __size[32];
#elif defined(__i386__)
	char __size[16];
#else
#error missing architecture specific code
#endif
	long __align;
} sem_t;

#define SEM_FAILED ((sem_t*) 0)

#define SEM_VALUE_MAX 2147483647

int sem_init(sem_t* __sem, int __pshared, unsigned int __value);
int sem_destroy(sem_t* __sem);
int sem_post(sem_t* __sem);
int sem_wait(sem_t* __sem);
int sem_trywait(sem_t* __sem);
int sem_timedwait(sem_t* __restrict __sem, const struct timespec* __restrict __abs_timeout);

__end_decls

#endif
