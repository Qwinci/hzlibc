#include "threads.h"
#include "utils.hpp"
#include "errno.h"
#include "ansi_sys.hpp"
#include "thread.hpp"

EXPORT int thrd_create(thrd_t* thread, thrd_start_t func, void* arg) {
	void* ptr = reinterpret_cast<void*>(func);
	auto new_func = reinterpret_cast<void* (*)(void*)>(ptr);

	int res = thread_create(thread, nullptr, new_func, arg);
	if (res == 0) {
		return thrd_success;
	}
	else if (res == ENOMEM) {
		return thrd_nomem;
	}
	else {
		return thrd_error;
	}
}

EXPORT int thrd_equal(thrd_t t1, thrd_t t2) {
	return thread_equal(t1, t2);
}

EXPORT thrd_t thrd_current() {
	return get_current_thread();
}

EXPORT int thrd_sleep(const timespec* duration, timespec* rem) {
#if UINTPTR_MAX == UINT64_MAX
	int err = sys_sleep(reinterpret_cast<const timespec64*>(duration), reinterpret_cast<timespec64*>(rem));
#else
	timespec64 duration64 {
		.tv_sec = duration->tv_sec,
		.tv_nsec = duration->tv_nsec
	};

	timespec64 rem64 {};

	int err = sys_sleep(&duration64, rem ? &rem64 : nullptr);
	if (rem) {
		rem->tv_sec = rem64.tv_sec;
		rem->tv_nsec = rem64.tv_nsec;
	}
#endif
	if (err == 0) {
		return 0;
	}
	else if (err == EINTR) {
		errno = EINTR;
		return -1;
	}
	else {
		errno = err;
		return -2;
	}
}

EXPORT void thrd_yield() {
	sys_sched_yield();
}

EXPORT __attribute__((noreturn)) void thrd_exit(int status) {
	hzlibc_thread_exit(reinterpret_cast<void*>(status));
}

EXPORT int thrd_detach(thrd_t thread) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	tcb->detached = true;
	return thrd_success;
}

EXPORT int thrd_join(thrd_t thread, int* status) {
	void* ret;
	int err = thread_join(thread, &ret);
	if (err == 0) {
		*status = static_cast<int>(reinterpret_cast<uintptr_t>(ret));
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int mtx_init(mtx_t* mtx, int type) {
	int pthread_type;
	if (type & mtx_recursive) {
		pthread_type = __HZLIBC_MUTEX_RECURSIVE;
	}
	else {
		pthread_type = __HZLIBC_MUTEX_NORMAL;
	}

	__hzlibc_mutexattr_t attr {};
	reinterpret_cast<MutexAttr*>(&attr)->type = pthread_type;
	int err = thread_mutex_init(&mtx->__mtx, &attr);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int mtx_lock(mtx_t* mtx) {
	int err = thread_mutex_lock(&mtx->__mtx);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int mtx_timedlock(mtx_t* __restrict mtx, const timespec* __restrict timeout) {
	__ensure(!"mtx_timedlock is not implemented");
}

EXPORT int mtx_trylock(mtx_t* mtx) {
	int err = thread_mutex_trylock(&mtx->__mtx);
	if (err == 0) {
		return thrd_success;
	}
	else if (err == EBUSY) {
		return thrd_busy;
	}
	else {
		return thrd_error;
	}
}

EXPORT int mtx_unlock(mtx_t* mtx) {
	int err = thread_mutex_unlock(&mtx->__mtx);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT void mtx_destroy(mtx_t* mtx) {
	thread_mutex_destroy(&mtx->__mtx);
}

EXPORT void call_once(once_flag* flag, void (*func)()) {
	thread_once(&flag->__once, func);
}

EXPORT int cnd_init(cnd_t* cnd) {
	int err = thread_cond_init(&cnd->__cnd, nullptr);
	if (err == 0) {
		return thrd_success;
	}
	else if (err == ENOMEM) {
		return thrd_nomem;
	}
	else {
		return thrd_error;
	}
}

EXPORT int cnd_signal(cnd_t* cnd) {
	int err = thread_cond_signal(&cnd->__cnd);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int cnd_broadcast(cnd_t* cnd) {
	int err = thread_cond_broadcast(&cnd->__cnd);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int cnd_wait(cnd_t* cnd, mtx_t* mtx) {
	int err = thread_cond_wait(&cnd->__cnd, &mtx->__mtx);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int cnd_timedwait(
	cnd_t* __restrict cnd,
	mtx_t* __restrict mtx,
	const struct timespec* __restrict timeout) {
	int err = thread_cond_timedwait(&cnd->__cnd, &mtx->__mtx, timeout);
	if (err == 0) {
		return thrd_success;
	}
	else if (err == ETIMEDOUT) {
		return thrd_timedout;
	}
	else {
		return thrd_error;
	}
}

EXPORT void cnd_destroy(cnd_t* cnd) {
	thread_cond_destroy(&cnd->__cnd);
}

EXPORT int tss_create(tss_t* key, tss_dtor_t destructor) {
	int err = thread_key_create(key, destructor);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT void* tss_get(tss_t key) {
	return thread_getspecific(key);
}

EXPORT int tss_set(tss_t key, void* value) {
	int err = thread_setspecific(key, value);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT void tss_delete(tss_t key) {
	thread_key_delete(key);
}
