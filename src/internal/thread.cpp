#include "thread.hpp"
#include "tcb.hpp"
#include "sys.hpp"
#include "rtld/rtld.hpp"
#include "errno.h"
#include "mutex.hpp"
#include <hz/array.hpp>

#if !ANSI_ONLY
void call_cxx_tls_destructors();
#endif

static void thread_call_destructors();

[[noreturn]] void hzlibc_thread_exit(void* ret) {
	auto* tcb = get_current_tcb();

#if !ANSI_ONLY
	call_cxx_tls_destructors();
#endif
	thread_call_destructors();

	if (tcb->detached) {
		__dlapi_destroy_tcb(tcb);
	}

	tcb->exit_status = ret;
	tcb->exited.store(1, hz::memory_order::release);
	sys_futex_wake_all(tcb->exited.data());

	sys_exit_thread();
}

extern "C" [[noreturn]] void hzlibc_thread_entry(void* (*fn)(void* arg), void* arg) {
#if !ANSI_ONLY
	auto* tcb = get_current_tcb();
	while (!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED)) {
		sys_futex_wait(&tcb->tid, 0, nullptr);
	}
#endif

	auto ret = fn(arg);
	hzlibc_thread_exit(ret);
}

int thread_create(
	__hzlibc_thread_t* __restrict thread,
	const __hzlibc_attr_t* __restrict attr,
	void* (*start_fn)(void* arg),
	void* __restrict arg) {
	void* tcb;
	void* tp;
	if (!__dlapi_create_tcb(&tcb, &tp)) {
		return ENOMEM;
	}

	void* stack_base = nullptr;
	size_t stack_size = 0x200000;

	auto tcb_ptr = static_cast<Tcb*>(tcb);

	if (attr) {
		auto* ptr = reinterpret_cast<const ThreadAttr*>(attr);
		if (ptr->stack_addr) {
			stack_base = ptr->stack_addr;
		}
		if (ptr->stack_size) {
			stack_size = ptr->stack_size;
		}
		if (ptr->detached) {
			tcb_ptr->detached = true;
		}
	}

	// todo propagate from sys_thread_create
	tcb_ptr->stack_addr = stack_base;
	tcb_ptr->stack_size = stack_size;
	tcb_ptr->guard_size = 0;

	pid_t tid;
	if (auto err = sys_thread_create(stack_base, stack_size, start_fn, arg, tp, &tid)) {
		__dlapi_destroy_tcb(tcb);
		return err;
	}

	*thread = reinterpret_cast<__hzlibc_thread_t>(tcb);

#if !ANSI_ONLY
	__atomic_store_n(&tcb_ptr->tid, tid, __ATOMIC_RELAXED);
	sys_futex_wake(&tcb_ptr->tid);
#endif

	return 0;
}

int thread_join(__hzlibc_thread_t thread, void** ret) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	if (tcb->detached) {
		return EINVAL;
	}

	while (!tcb->exited.load(hz::memory_order::acquire)) {
		sys_futex_wait(tcb->exited.data(), 0, nullptr);
	}

	if (ret) {
		*ret = tcb->exit_status;
	}
	__dlapi_destroy_tcb(tcb);
	return 0;
}

int thread_mutex_init(__hzlibc_mutex_t* __restrict mutex, const __hzlibc_mutexattr_t* __restrict attr) {
	auto* ptr = reinterpret_cast<Mutex*>(mutex);
	new (ptr) Mutex {};

	if (attr) {
		auto* attr_ptr = reinterpret_cast<const MutexAttr*>(attr);
		ptr->state.kind = attr_ptr->type;
	}
	return 0;
}

int thread_mutex_destroy(__hzlibc_mutex_t* mutex) {
	return 0;
}

int thread_mutex_lock(__hzlibc_mutex_t* mutex) {
	return reinterpret_cast<Mutex*>(mutex)->manual_lock();
}

int thread_mutex_trylock(__hzlibc_mutex_t* mutex) {
	return reinterpret_cast<Mutex*>(mutex)->try_manual_lock();
}

int thread_mutex_unlock(__hzlibc_mutex_t* mutex) {
	return reinterpret_cast<Mutex*>(mutex)->manual_unlock();
}

namespace {
	constexpr int ONCE_INCOMPLETE = 0;
	constexpr int ONCE_QUEUED = 1;
	constexpr int ONCE_RUNNING = 2;
	constexpr int ONCE_COMPLETE = 3;
}

int thread_once(__hzlibc_once_t* once, void (*init_fn)()) {
	auto* ptr = reinterpret_cast<hz::atomic<int>*>(once);
	auto state = ptr->load(hz::memory_order::acquire);
	while (true) {
		if (state == ONCE_INCOMPLETE) {
			if (!ptr->compare_exchange_weak(
				state,
				ONCE_RUNNING,
				hz::memory_order::acquire,
				hz::memory_order::acquire
			)) {
				continue;
			}

			init_fn();
			if (ptr->exchange(ONCE_COMPLETE, hz::memory_order::release) == ONCE_QUEUED) {
				sys_futex_wake_all(ptr->data());
			}
			return 0;
		}
		else if (state == ONCE_RUNNING || state == ONCE_QUEUED) {
			if (state == ONCE_RUNNING &&
			    !ptr->compare_exchange_weak(
				    state,
				    ONCE_QUEUED,
				    hz::memory_order::relaxed,
				    hz::memory_order::acquire
			    )) {
				continue;
			}
			sys_futex_wait(ptr->data(), ONCE_QUEUED, nullptr);
			state = ptr->load(hz::memory_order::acquire);
		}
		else {
			return 0;
		}
	}
}

struct Cond {
	hz::atomic<int> futex {};
	clockid_t clock {CLOCK_REALTIME};
};
static_assert(sizeof(Cond) <= sizeof(__hzlibc_cond_t));

int thread_cond_init(__hzlibc_cond_t* __restrict cond, const __hzlibc_condattr_t* __restrict attr) {
	if (attr) {
		new (cond) Cond {
			.clock = reinterpret_cast<const CondAttr*>(attr)->clock
		};
	}
	else {
		new (cond) Cond {};
	}
	return 0;
}

int thread_cond_destroy(__hzlibc_cond_t* cond) {
	return 0;
}

int thread_cond_wait(__hzlibc_cond_t* __restrict cond, __hzlibc_mutex_t* __restrict mutex) {
	auto* ptr = reinterpret_cast<Cond*>(cond);
	auto value = ptr->futex.load(hz::memory_order::relaxed);
	thread_mutex_unlock(mutex);
	sys_futex_wait(ptr->futex.data(), value, nullptr);
	thread_mutex_lock(mutex);
	return 0;
}

int thread_cond_timedwait(
	__hzlibc_cond_t* __restrict cond,
	__hzlibc_mutex_t* __restrict mutex,
	const timespec* __restrict abs_timeout) {
	if (!abs_timeout) {
		return thread_cond_wait(cond, mutex);
	}
	else {
		auto* ptr = reinterpret_cast<Cond*>(cond);

		while (true) {
			timespec current {};
			if (auto err = sys_clock_gettime(ptr->clock, &current)) {
				return err;
			}

			timespec timeout {
				.tv_sec = abs_timeout->tv_sec - current.tv_sec,
				.tv_nsec = abs_timeout->tv_nsec - current.tv_nsec
			};
			if (timeout.tv_sec < 0 || (timeout.tv_sec == 0 && timeout.tv_nsec < 0)) {
				return ETIMEDOUT;
			}

			auto value = ptr->futex.load(hz::memory_order::relaxed);
			thread_mutex_unlock(mutex);
			int err = sys_futex_wait(ptr->futex.data(), value, &timeout);
			thread_mutex_lock(mutex);
			if (err == ETIMEDOUT) {
				return ETIMEDOUT;
			}
			else if (err == EINTR) {
				continue;
			}
			else {
				return 0;
			}
		}
	}
}

int thread_cond_broadcast(__hzlibc_cond_t* cond) {
	auto* ptr = reinterpret_cast<Cond*>(cond);
	ptr->futex.fetch_add(1, hz::memory_order::relaxed);
	sys_futex_wake_all(ptr->futex.data());
	return 0;
}

int thread_cond_signal(__hzlibc_cond_t* cond) {
	auto* ptr = reinterpret_cast<Cond*>(cond);
	ptr->futex.fetch_add(1, hz::memory_order::relaxed);
	sys_futex_wake(ptr->futex.data());
	return 0;
}

namespace {
	struct Key {
		void (*destructor)(void* arg) {};
		int generation {};
		bool used {};
	};

	hz::array<Key, PTHREAD_KEYS_MAX> KEYS {};
	Mutex KEYS_MUTEX {};
}

static void thread_call_destructors() {
	auto* tcb = get_current_tcb();
	for (size_t iterations = 0; iterations < PTHREAD_DESTRUCTOR_ITERATIONS; ++iterations) {
		for (size_t i = 0; i < PTHREAD_KEYS_MAX; ++i) {
			auto& local = tcb->keys[i];
			void (*destructor)(void*);
			{
				auto guard = KEYS_MUTEX.lock();
				auto& global = KEYS[i];
				if (!global.used || global.generation != local.generation ||
				    !local.value) {
					continue;
				}
				destructor = global.destructor;
			}

			if (destructor) {
				auto value = local.value;
				local.value = nullptr;
				destructor(value);
			}
		}
	}
}

int thread_key_create(__hzlibc_key_t* key, void (*destructor)(void* arg)) {
	auto guard = KEYS_MUTEX.lock();

	for (size_t i = 0; i < PTHREAD_KEYS_MAX; ++i) {
		auto& key_value = KEYS[i];
		if (!key_value.used) {
			key_value.used = true;
			key_value.destructor = destructor;
			++key_value.generation;
			*key = i;
			return 0;
		}
	}

	return EAGAIN;
}

int thread_key_delete(__hzlibc_key_t key) {
	if (key >= PTHREAD_KEYS_MAX) {
		return EINVAL;
	}

	auto guard = KEYS_MUTEX.lock();
	auto& key_value = KEYS[key];
	if (!key_value.used) {
		return EINVAL;
	}

	key_value.used = false;
	key_value.destructor = nullptr;
	return 0;
}

void* thread_getspecific(__hzlibc_key_t key) {
	auto guard = KEYS_MUTEX.lock();
	if (key >= PTHREAD_KEYS_MAX || !KEYS[key].used) {
		panic("pthread_getspecific: invalid key");
	}

	auto& global_key = KEYS[key];

	auto* tcb = get_current_tcb();
	auto& local_key = tcb->keys[key];
	if (local_key.generation != global_key.generation) {
		local_key.generation = global_key.generation;
		local_key.value = nullptr;
	}
	return local_key.value;
}

int thread_setspecific(__hzlibc_key_t key, const void* value) {
	auto guard = KEYS_MUTEX.lock();
	if (key >= PTHREAD_KEYS_MAX || !KEYS[key].used) {
		return EINVAL;
	}

	auto& global_key = KEYS[key];

	auto* tcb = get_current_tcb();
	auto& local_key = tcb->keys[key];
	if (local_key.generation != global_key.generation) {
		local_key.generation = global_key.generation;
	}
	local_key.value = const_cast<void*>(value);
	return 0;
}
