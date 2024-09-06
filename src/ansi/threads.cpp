#include "threads.h"
#include "utils.hpp"
#include "errno.h"

EXPORT int thrd_create(thrd_t* thread, thrd_start_t func, void* arg) {
	void* ptr = reinterpret_cast<void*>(func);
	auto new_func = reinterpret_cast<void* (*)(void*)>(ptr);

	int res = pthread_create(thread, nullptr, new_func, arg);
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
	return pthread_equal(t1, t2);
}

EXPORT thrd_t thrd_current() {
	return pthread_self();
}

EXPORT int thrd_sleep(const timespec* duration, timespec* rem) {
	int err = nanosleep(duration, rem);
	if (err == 0) {
		return 0;
	}
	else if (err == -1 && errno == EINTR) {
		return -1;
	}
	else {
		return -2;
	}
}

EXPORT void thrd_yield() {
	sched_yield();
}

EXPORT __attribute__((noreturn)) void thrd_exit(int status) {
	pthread_exit(reinterpret_cast<void*>(status));
}

EXPORT int thrd_detach(thrd_t thread) {
	int err = pthread_detach(thread);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int thrd_join(thrd_t thread, int* status) {
	void* ret;
	int err = pthread_join(thread, &ret);
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
		pthread_type = PTHREAD_MUTEX_RECURSIVE;
	}
	else {
		pthread_type = PTHREAD_MUTEX_NORMAL;
	}

	pthread_mutexattr_t attr {};
	pthread_mutexattr_settype(&attr, pthread_type);
	int err = pthread_mutex_init(&mtx->__mtx, nullptr);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int mtx_lock(mtx_t* mtx) {
	int err = pthread_mutex_lock(&mtx->__mtx);
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
	int err = pthread_mutex_trylock(&mtx->__mtx);
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
	int err = pthread_mutex_unlock(&mtx->__mtx);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT void mtx_destroy(mtx_t* mtx) {
	pthread_mutex_destroy(&mtx->__mtx);
}

EXPORT void call_once(once_flag* flag, void (*func)()) {
	pthread_once(&flag->__data, func);
}

EXPORT int cnd_init(cnd_t* cnd) {
	int err = pthread_cond_init(&cnd->__cnd, nullptr);
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
	int err = pthread_cond_signal(&cnd->__cnd);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int cnd_broadcast(cnd_t* cnd) {
	int err = pthread_cond_broadcast(&cnd->__cnd);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT int cnd_wait(cnd_t* cnd, mtx_t* mtx) {
	int err = pthread_cond_wait(&cnd->__cnd, &mtx->__mtx);
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
	int err = pthread_cond_timedwait(&cnd->__cnd, &mtx->__mtx, timeout);
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
	pthread_cond_destroy(&cnd->__cnd);
}

EXPORT int tss_create(tss_t* key, tss_dtor_t destructor) {
	int err = pthread_key_create(key, destructor);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT void* tss_get(tss_t key) {
	return pthread_getspecific(key);
}

EXPORT int tss_set(tss_t key, void* value) {
	int err = pthread_setspecific(key, value);
	if (err == 0) {
		return thrd_success;
	}
	else {
		return thrd_error;
	}
}

EXPORT void tss_delete(tss_t key) {
	pthread_key_delete(key);
}
