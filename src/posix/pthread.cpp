#include "pthread.h"
#include "signal.h"
#include "utils.hpp"
#include "sys.hpp"
#include "internal/tcb.hpp"
#include "rtld/rtld.hpp"
#include "errno.h"
#include "mutex.hpp"
#include <hz/array.hpp>
#include <limits.h>

EXPORT int pthread_create(
	pthread_t* __restrict thread,
	const pthread_attr_t* __restrict attr,
	void* (*start_fn)(void* arg),
	void* __restrict arg) {
	void* tcb;
	void* tp;
	if (!__dlapi_create_tcb(&tcb, &tp)) {
		errno = ENOMEM;
		return -1;
	}

	void* stack_base = nullptr;
	size_t stack_size = 0x200000;

	pid_t tid;
	if (auto err = sys_thread_create(stack_base, stack_size, start_fn, arg, tp, &tid)) {
		errno = err;
		__dlapi_destroy_tcb(tcb);
		return -1;
	}

	*thread = reinterpret_cast<pthread_t>(tcb);

	auto* tcb_ptr = static_cast<Tcb*>(tcb);
	__atomic_store_n(&tcb_ptr->tid, tid, __ATOMIC_RELAXED);
	sys_futex_wake(&tcb_ptr->tid);

	return 0;
}

EXPORT int pthread_join(pthread_t thread, void** ret) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);

	while (!tcb->exited.load(hz::memory_order::acquire)) {
		sys_futex_wait(tcb->exited.data(), 0, nullptr);
	}

	if (ret) {
		*ret = tcb->exit_status;
	}
	__dlapi_destroy_tcb(tcb);
	return 0;
}

namespace {
	struct Key {
		void (*destructor)(void* arg) {};
		bool used {};
	};

	hz::array<Key, PTHREAD_KEYS_MAX> KEYS {};
	Mutex KEYS_MUTEX {};
}

EXPORT int pthread_key_create(pthread_key_t* key, void (*destructor)(void* arg)) {
	auto guard = KEYS_MUTEX.lock();

	for (size_t i = 0; i < PTHREAD_KEYS_MAX; ++i) {
		auto& key_value = KEYS[i];
		if (!key_value.used) {
			key_value.used = true;
			key_value.destructor = destructor;
			*key = i;
			return 0;
		}
	}

	errno = EAGAIN;
	return -1;
}

EXPORT int pthread_key_delete(pthread_key_t key) {
	if (key >= PTHREAD_KEYS_MAX) {
		errno = EINVAL;
		return -1;
	}

	auto guard = KEYS_MUTEX.lock();
	auto& key_value = KEYS[key];
	if (!key_value.used) {
		errno = EINVAL;
		return -1;
	}

	key_value.used = false;
	key_value.destructor = nullptr;
	return 0;
}

EXPORT int pthread_sigmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old) {
	if (auto err = sys_sigprocmask(how, set, old)) {
		return err;
	}
	return 0;
}

ALIAS(pthread_key_create, __pthread_key_create);
