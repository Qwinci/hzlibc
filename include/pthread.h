#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <bits/utils.h>
#include <bits/thread_types.h>
#include <stddef.h>
#include <time.h>
#include <sched.h>

__begin_decls

typedef __hzlibc_thread_t pthread_t;
typedef __hzlibc_key_t pthread_key_t;
typedef __hzlibc_once_t pthread_once_t;
typedef __hzlibc_spinlock_t pthread_spinlock_t;

enum {
	PTHREAD_MUTEX_NORMAL = __HZLIBC_MUTEX_NORMAL,
	PTHREAD_MUTEX_RECURSIVE = __HZLIBC_MUTEX_RECURSIVE,
	PTHREAD_MUTEX_ERRORCHECK = __HZLIBC_MUTEX_ERRORCHECK
};

#define PTHREAD_ONCE_INIT __HZLIBC_ONCE_INIT
#define PTHREAD_MUTEX_INITIALIZER __HZLIBC_MUTEX_INITIALIZER
#define PTHREAD_COND_INITIALIZER __HZLIBC_COND_INITIALIZER

#define PTHREAD_CANCEL_ENABLE __HZLIBC_CANCEL_ENABLE
#define PTHREAD_CANCEL_DISABLE __HZLIBC_CANCEL_DISABLE

#define PTHREAD_CANCEL_DEFERRED __HZLIBC_CANCEL_DEFERRED
#define PTHREAD_CANCEL_ASYNCHRONOUS __HZLIBC_CANCEL_ASYNCHRONOUS

#define PTHREAD_CREATE_JOINABLE __HZLIBC_THREAD_CREATE_JOINABLE
#define PTHREAD_CREATE_DETACHED __HZLIBC_THREAD_CREATE_DETACHED

#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1

#define PTHREAD_STACK_MIN __HZLIBC_THREAD_STACK_MIN

#define PTHREAD_RWLOCK_INITIALIZER __HZLIBC_RWLOCK_INITIALIZER

#define PTHREAD_BARRIER_SERIAL_THREAD __HZLIBC_BARRIER_SERIAL_THREAD

typedef __hzlibc_mutex_t pthread_mutex_t;
typedef __hzlibc_cond_t pthread_cond_t;
typedef __hzlibc_rwlock_t pthread_rwlock_t;
typedef __hzlibc_barrier_t pthread_barrier_t;
typedef __hzlibc_mutexattr_t pthread_mutexattr_t;
typedef __hzlibc_condattr_t pthread_condattr_t;
typedef __hzlibc_attr_t pthread_attr_t;
typedef __hzlibc_rwlockattr_t pthread_rwlockattr_t;
typedef __hzlibc_barrierattr_t pthread_barrierattr_t;

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
int pthread_cancel(pthread_t __thrd);

int pthread_atfork(void (*__prepare)(void), void (*__parent)(void), void (*__child)(void));

int pthread_key_create(pthread_key_t* __key, void (*__destructor)(void* __arg));
int pthread_key_delete(pthread_key_t __key);

void* pthread_getspecific(pthread_key_t __key);
int pthread_setspecific(pthread_key_t __key, const void* __value);

int pthread_setschedparam(pthread_t __thrd, int __policy, const struct sched_param* __param);
int pthread_getschedparam(pthread_t __thrd, int* __policy, struct sched_param* __param);

int pthread_rwlock_init(pthread_rwlock_t* __restrict __lock, const pthread_rwlockattr_t* __attr);
int pthread_rwlock_destroy(pthread_rwlock_t* __lock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t* __lock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t* __restrict __lock, const struct timespec* __restrict __abs_timeout);
int pthread_rwlock_rdlock(pthread_rwlock_t* __lock);
int pthread_rwlock_trywrlock(pthread_rwlock_t* __lock);
int pthread_rwlock_timedwrlock(pthread_rwlock_t* __restrict __lock, const struct timespec* __restrict __abs_timeout);
int pthread_rwlock_wrlock(pthread_rwlock_t* __lock);
int pthread_rwlock_unlock(pthread_rwlock_t* __lock);

int pthread_mutex_init(pthread_mutex_t* __restrict __mutex, const pthread_mutexattr_t* __restrict __attr);
int pthread_mutex_destroy(pthread_mutex_t* __mutex);
int pthread_mutex_lock(pthread_mutex_t* __mutex);
int pthread_mutex_trylock(pthread_mutex_t* __mutex);
int pthread_mutex_timedlock(pthread_mutex_t* __restrict __mutex, const struct timespec* __restrict __abs_timeout);
int pthread_mutex_unlock(pthread_mutex_t* __mutex);

int pthread_once(pthread_once_t* __once, void (*__init_fn)(void));

int pthread_getcpuclockid(pthread_t __thrd, clockid_t* __clock_id);

int pthread_cond_init(pthread_cond_t* __restrict __cond, const pthread_condattr_t* __restrict __attr);
int pthread_cond_destroy(pthread_cond_t* __cond);
int pthread_cond_wait(pthread_cond_t* __restrict __cond, pthread_mutex_t* __restrict __mutex);
int pthread_cond_timedwait(
	pthread_cond_t* __restrict __cond,
	pthread_mutex_t* __restrict __mutex,
	const struct timespec* __restrict __abs_timeout);
int pthread_cond_clockwait(
	pthread_cond_t* __restrict __cond,
	pthread_mutex_t* __restrict __mutex,
	clockid_t __clock_id,
	const struct timespec* __restrict __abs_timeout);
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
int pthread_attr_setschedpolicy(pthread_attr_t* __restrict __attr, int __policy);
int pthread_attr_setschedparam(pthread_attr_t* __restrict __attr, const struct sched_param* __restrict __param);
int pthread_attr_setinheritsched(pthread_attr_t* __attr, int __inherit_sched);

int pthread_mutexattr_init(pthread_mutexattr_t* __attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t* __attr);
int pthread_mutexattr_gettype(const pthread_mutexattr_t* __restrict __attr, int* __restrict __type);
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
