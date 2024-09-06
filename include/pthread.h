#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <bits/utils.h>
#include <stddef.h>
#include <time.h>
#include <sched.h>

__begin_decls

typedef unsigned long pthread_t;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;
typedef volatile int pthread_spinlock_t;

enum {
	PTHREAD_MUTEX_NORMAL,
	PTHREAD_MUTEX_RECURSIVE,
	PTHREAD_MUTEX_ERRORCHECK
};

#define PTHREAD_ONCE_INIT 0

#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

#define PTHREAD_STACK_MIN 16384

#define PTHREAD_RWLOCK_INITIALIZER {}

#define PTHREAD_BARRIER_SERIAL_THREAD -1

typedef union {
#ifdef __x86_64__
	char __size[40];
#elif defined(__i386__)
	char __size[24];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_mutex_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[48];
#else
#error missing architecture specific code
#endif
	long long __align;
} pthread_cond_t;

typedef union {
#ifdef __x86_64__
	char __size[56];
#elif defined(__i386__)
	char __size[32];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_rwlock_t;

typedef union {
#ifdef __x86_64__
	char __size[32];
#elif defined(__i386__)
	char __size[20];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_barrier_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	int __align;
} pthread_mutexattr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	int __align;
} pthread_condattr_t;

typedef union {
#ifdef __x86_64__
	char __size[56];
#elif defined(__i386__)
	char __size[36];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_attr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[8];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_rwlockattr_t;

typedef union {
#if defined(__x86_64__) || defined(__i386__)
	char __size[4];
#else
#error missing architecture specific code
#endif
	long __align;
} pthread_barrierattr_t;

pthread_t pthread_self(void);
int pthread_equal(pthread_t __t1, pthread_t __t2);

int pthread_create(
	pthread_t* __restrict __thrd,
	const pthread_attr_t* __restrict __attr,
	void* (*__start_fn)(void* __arg),
	void* __restrict __arg);
int pthread_join(pthread_t __thrd, void** __ret);
int pthread_detach(pthread_t __thrd);
int pthread_kill(pthread_t __thrd, int __sig);
__attribute__((noreturn)) void pthread_exit(void* __ret);
int pthread_setcanceltype(int __state, int* __old_state);

int pthread_atfork(void (*__prepare)(void), void (*__parent)(void), void (*__child)(void));

int pthread_key_create(pthread_key_t* __key, void (*__destructor)(void* __arg));
int pthread_key_delete(pthread_key_t __key);

void* pthread_getspecific(pthread_key_t __key);
int pthread_setspecific(pthread_key_t __key, const void* __value);

int pthread_setschedparam(pthread_t __thrd, int __policy, const struct sched_param* __param);
int pthread_getschedparam(pthread_t __thrd, int* __policy, struct sched_param* __param);

int pthread_rwlock_init(pthread_rwlock_t* __restrict __lock, const pthread_rwlockattr_t* __attr);
int pthread_rwlock_destroy(pthread_rwlock_t* __lock);
int pthread_rwlock_rdlock(pthread_rwlock_t* __lock);
int pthread_rwlock_wrlock(pthread_rwlock_t* __lock);
int pthread_rwlock_unlock(pthread_rwlock_t* __lock);

int pthread_mutex_init(pthread_mutex_t* __restrict __mutex, const pthread_mutexattr_t* __restrict __attr);
int pthread_mutex_destroy(pthread_mutex_t* __mutex);
int pthread_mutex_lock(pthread_mutex_t* __mutex);
int pthread_mutex_trylock(pthread_mutex_t* __mutex);
int pthread_mutex_unlock(pthread_mutex_t* __mutex);

int pthread_once(pthread_once_t* __once, void (*__init_fn)(void));

int pthread_getcpuclockid(pthread_t __thrd, clockid_t* __clock_id);

int pthread_cond_init(pthread_cond_t* __restrict __cond, const pthread_condattr_t* __restrict __attr);
int pthread_cond_destroy(pthread_cond_t* __cond);
int pthread_cond_wait(pthread_cond_t* __restrict __cond, pthread_mutex_t* __restrict __mutex);
int pthread_cond_timedwait(
	pthread_cond_t* __restrict __cond,
	pthread_mutex_t* __restrict __mutex,
	const struct timespec* __restrict __abs_time);
int pthread_cond_broadcast(pthread_cond_t* __cond);
int pthread_cond_signal(pthread_cond_t* __cond);

int pthread_barrier_init(
	pthread_barrier_t* __restrict __barrier,
	const pthread_barrierattr_t* __restrict __attr,
	unsigned int __count);
int pthread_barrier_destroy(pthread_barrier_t* __restrict __barrier);
int pthread_barrier_wait(pthread_barrier_t* __barrier);

int pthread_attr_init(pthread_attr_t* __attr);
int pthread_attr_destroy(pthread_attr_t* __attr);
int pthread_attr_setdetachstate(pthread_attr_t* __attr, int __detach_state);
int pthread_attr_setstacksize(pthread_attr_t* __attr, size_t __stack_size);

int pthread_mutexattr_init(pthread_mutexattr_t* __attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t* __attr);
int pthread_mutexattr_settype(pthread_mutexattr_t* __attr, int __type);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t* __attr, int __protocol);

int pthread_condattr_init(pthread_condattr_t* __attr);
int pthread_condattr_destroy(pthread_condattr_t* __attr);
int pthread_condattr_setclock(pthread_condattr_t* __attr, clockid_t __clock_id);

// glibc
int pthread_setname_np(pthread_t __thrd, const char* __name);
int pthread_getname_np(pthread_t __thrd, char* __name, size_t __len);

int pthread_setaffinity_np(pthread_t __thrd, size_t __cpu_set_size, const cpu_set_t* __cpu_set);
int pthread_getaffinity_np(pthread_t __thrd, size_t __cpu_set_size, cpu_set_t* __cpu_set);

int pthread_num_processors_np(void);

__end_decls

#endif
