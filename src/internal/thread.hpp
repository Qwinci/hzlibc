#pragma once
#include "tcb.hpp"
#include "bits/thread_types.h"
#include "time.h"

[[noreturn]] void hzlibc_thread_exit(void* ret);
extern "C" [[noreturn]] void hzlibc_thread_entry(void* (*fn)(void* arg), void* arg);

struct ThreadAttr {
	size_t stack_size;
	int policy;
	int priority;
	int inherit_sched;
	bool detached;
};
static_assert(sizeof(ThreadAttr) <= sizeof(__hzlibc_attr_t));
struct MutexAttr {
	int type : 4;
};
static_assert(sizeof(MutexAttr) <= sizeof(__hzlibc_mutexattr_t));
struct CondAttr {
	clockid_t clock;
};
static_assert(sizeof(CondAttr) <= sizeof(__hzlibc_condattr_t));

inline __hzlibc_thread_t get_current_thread() {
	return reinterpret_cast<__hzlibc_thread_t>(get_current_tcb());
}

inline bool thread_equal(__hzlibc_thread_t a, __hzlibc_thread_t b) {
	return a == b;
}

int thread_create(
	__hzlibc_thread_t* __restrict thread,
	const __hzlibc_attr_t* __restrict attr,
	void* (*start_fn)(void* arg),
	void* __restrict arg);
int thread_join(__hzlibc_thread_t thread, void** ret);

int thread_mutex_init(__hzlibc_mutex_t* __restrict mutex, const __hzlibc_mutexattr_t* __restrict attr);
int thread_mutex_destroy(__hzlibc_mutex_t* mutex);
int thread_mutex_lock(__hzlibc_mutex_t* mutex);
int thread_mutex_trylock(__hzlibc_mutex_t* mutex);
int thread_mutex_unlock(__hzlibc_mutex_t* mutex);

int thread_once(__hzlibc_once_t* once, void (*init_fn)());

int thread_cond_init(__hzlibc_cond_t* __restrict cond, const __hzlibc_condattr_t* __restrict attr);
int thread_cond_destroy(__hzlibc_cond_t* cond);
int thread_cond_wait(__hzlibc_cond_t* __restrict cond, __hzlibc_mutex_t* __restrict mutex);
int thread_cond_timedwait(
	__hzlibc_cond_t* __restrict cond,
	__hzlibc_mutex_t* __restrict mutex,
	const timespec* __restrict abs_timeout);
int thread_cond_broadcast(__hzlibc_cond_t* cond);
int thread_cond_signal(__hzlibc_cond_t* cond);

int thread_key_create(__hzlibc_key_t* key, void (*destructor)(void* arg));
int thread_key_delete(__hzlibc_key_t key);
void* thread_getspecific(__hzlibc_key_t key);
int thread_setspecific(__hzlibc_key_t key, const void* value);
