#ifndef _THREADS_H
#define _THREADS_H

#include <bits/utils.h>
#include <bits/thread_types.h>

__begin_decls

typedef __hzlibc_thread_t thrd_t;
typedef int (*thrd_start_t)(void* __arg);

enum {
	thrd_success = 0,
	thrd_busy = 1,
	thrd_error = 2,
	thrd_nomem = 3,
	thrd_timedout = 4
};

enum {
	mtx_plain = 0,
	mtx_recursive = 1,
	mtx_timed = 2
};

typedef union {
	__hzlibc_mutex_t __mtx;
} mtx_t;


typedef struct {
	__hzlibc_once_t __once;
} once_flag;

#define ONCE_FLAG_INIT {0}

typedef union {
	__hzlibc_cond_t __cnd;
} cnd_t;

#define TSS_DTOR_ITERATIONS 4
typedef unsigned int tss_t;
typedef void (*tss_dtor_t)(void* __arg);

#ifndef __cplusplus
#define thread_local _Thread_local
#endif

int thrd_create(thrd_t* __thrd, thrd_start_t __func, void* __arg);
int thrd_equal(thrd_t __t1, thrd_t __t2);
thrd_t thrd_current(void);
int thrd_sleep(const struct timespec* __duration, struct timespec* __rem);
void thrd_yield(void);
__attribute__((noreturn)) void thrd_exit(int __status);
int thrd_detach(thrd_t __thrd);
int thrd_join(thrd_t __thrd, int* __status);

int mtx_init(mtx_t* __mtx, int __type);
int mtx_lock(mtx_t* __mtx);
int mtx_timedlock(mtx_t* __restrict __mtx, const struct timespec* __restrict __timeout);
int mtx_trylock(mtx_t* __mtx);
int mtx_unlock(mtx_t* __mtx);
void mtx_destroy(mtx_t* __mtx);

void call_once(once_flag* __flag, void (*__func)(void));

int cnd_init(cnd_t* __cnd);
int cnd_signal(cnd_t* __cnd);
int cnd_broadcast(cnd_t* __cnd);
int cnd_wait(cnd_t* __cnd, mtx_t* __mtx);
int cnd_timedwait(
	cnd_t* __restrict __cnd,
	mtx_t* __restrict __mtx,
	const struct timespec* __restrict __timeout);
void cnd_destroy(cnd_t* __cnd);

int tss_create(tss_t* __key, tss_dtor_t __destructor);
void* tss_get(tss_t __key);
int tss_set(tss_t __key, void* __value);
void tss_delete(tss_t __key);

__end_decls

#endif
