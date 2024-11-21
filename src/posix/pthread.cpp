#include "pthread.h"
#include "signal.h"
#include "utils.hpp"
#include "sys.hpp"
#include "internal/thread.hpp"
#include "rtld/rtld.hpp"
#include "errno.h"
#include "mutex.hpp"
#include <hz/array.hpp>
#include <hz/new.hpp>
#include <limits.h>

EXPORT pthread_t pthread_self() {
	return get_current_thread();
}

EXPORT int pthread_equal(pthread_t t1, pthread_t t2) {
	return thread_equal(t1, t2);
}

EXPORT int pthread_create(
	pthread_t* __restrict thread,
	const pthread_attr_t* __restrict attr,
	void* (*start_fn)(void* arg),
	void* __restrict arg) {
	return thread_create(thread, attr, start_fn, arg);
}

EXPORT int pthread_join(pthread_t thread, void** ret) {
	return thread_join(thread, ret);
}

EXPORT int pthread_detach(pthread_t thread) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	tcb->detached = true;
	return 0;
}

EXPORT int pthread_kill(pthread_t thread, int sig) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);

	auto pid = sys_get_process_id();

	if (auto err = sys_tgkill(pid, tcb->tid, sig)) {
		return err;
	}

	return 0;
}

EXPORT __attribute__((noreturn)) void pthread_exit(void* ret) {
	hzlibc_thread_exit(ret);
}

EXPORT int pthread_setcanceltype(int state, int* old_state) {
	println("pthread_setcanceltype: pthread cancellation is not implemented");
	return 0;
}

EXPORT int pthread_cancel(pthread_t thread) {
	println("pthread_cancel: pthread cancellation is not implemented");
	return 0;
}

extern "C" int __register_atfork(
	void (*prepare)(),
	void (*parent)(),
	void (*child)(),
	void* dso_handle);

EXPORT int pthread_atfork(void (*prepare)(), void (*parent)(), void (*child)()) {
	return __register_atfork(prepare, parent, child, nullptr);
}

EXPORT int pthread_key_create(pthread_key_t* key, void (*destructor)(void* arg)) {
	return thread_key_create(key, destructor);
}

EXPORT int pthread_key_delete(pthread_key_t key) {
	return thread_key_delete(key);
}

EXPORT void* pthread_getspecific(pthread_key_t key) {
	return thread_getspecific(key);
}

EXPORT int pthread_setspecific(pthread_key_t key, const void* value) {
	return thread_setspecific(key, value);
}

EXPORT int pthread_setschedparam(pthread_t thread, int policy, const sched_param* param) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	if (auto err = sys_sched_setscheduler(tcb->tid, policy, param)) {
		return err;
	}
	return 0;
}

EXPORT int pthread_getschedparam(pthread_t thread, int* policy, sched_param* param) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	if (auto err = sys_sched_getscheduler(tcb->tid, policy)) {
		return err;
	}
	if (auto err = sys_sched_getparam(tcb->tid, param)) {
		return err;
	}
	return 0;
}

struct RwLock {
#if defined(__x86_64__) || defined(__aarch64__)
	hz::atomic<int> state;
	hz::atomic<int> writer_notify;
	int pad0[8];
	unsigned long pad1;
	unsigned int flags;
#elif defined(__i386__)
	hz::atomic<int> state;
	hz::atomic<int> writer_notify;
	unsigned int pad0[4];
	unsigned char flags;
	unsigned char pad2[7];
#else
#error missing architecture specific code
#endif
};
static_assert(sizeof(RwLock) <= sizeof(pthread_rwlock_t));

namespace {
	constexpr int WRITERS_WAITING = 1 << 31;
	constexpr int READERS_WAITING = 1 << 30;
	constexpr int READERS_MASK = (1 << 30) - 1;
}

EXPORT int pthread_rwlock_init(pthread_rwlock_t* __restrict lock, const pthread_rwlockattr_t* attr) {
	new (lock) RwLock {};
	return 0;
}

EXPORT int pthread_rwlock_destroy(pthread_rwlock_t*) {
	return 0;
}

namespace {
	constexpr int MUTEX_OWNER_MASK = (1 << 30) - 1;
	constexpr int MUTEX_WAITERS_BIT = 1 << 31;
	constexpr int MUTEX_EXCLUSIVE_BIT = 1 << 30;

	constexpr int RC_COUNT_MASK = (1 << 30) - 1;
	constexpr int RC_WAITERS_BIT = (1 << 31);

	void rwlock_mutex_lock(RwLock* lock, bool exclusive) {
		int expected = lock->state.load(hz::memory_order::relaxed);
		while (true) {
			if (expected) {
				__ensure(expected & MUTEX_OWNER_MASK);

				if (!(expected & MUTEX_WAITERS_BIT)) {
					if (!lock->state.compare_exchange_weak(
						expected,
						expected | MUTEX_WAITERS_BIT,
						hz::memory_order::relaxed,
						hz::memory_order::relaxed
					)) {
						continue;
					}
				}

				sys_futex_wait(lock->state.data(), expected | MUTEX_WAITERS_BIT, nullptr);
				expected = 0;
			}
			else {
				int desired = 1;
				if (exclusive) {
					desired |= MUTEX_EXCLUSIVE_BIT;
				}
				if (lock->state.compare_exchange_weak(
					expected,
					desired,
					hz::memory_order::acquire,
					hz::memory_order::relaxed
					)) {
					break;
				}
			}
		}
	}

	void rwlock_mutex_unlock(RwLock* lock) {
		auto state = lock->state.exchange(0, hz::memory_order::release);
		if (state & MUTEX_WAITERS_BIT) {
			sys_futex_wake_all(lock->state.data());
		}
	}
}

EXPORT int pthread_rwlock_tryrdlock(pthread_rwlock_t* lock) {
	__ensure(!"pthread_rwlock_tryrdlock is not implemented");
}

EXPORT int pthread_rwlock_timedrdlock(pthread_rwlock_t* __restrict lock, const timespec* __restrict abs_timeout) {
	__ensure(!"pthread_rwlock_timedrdlock is not implemented");
}

EXPORT int pthread_rwlock_rdlock(pthread_rwlock_t* lock) {
	auto* ptr = reinterpret_cast<RwLock*>(lock);

	rwlock_mutex_lock(ptr, false);
	ptr->writer_notify.fetch_add(1, hz::memory_order::acquire);
	rwlock_mutex_unlock(ptr);

	return 0;
}

EXPORT int pthread_rwlock_trywrlock(pthread_rwlock_t* lock) {
	__ensure(!"pthread_rwlock_trywrlock is not implemented");
}

EXPORT int pthread_rwlock_timedwrlock(pthread_rwlock_t* __restrict lock, const timespec* __restrict abs_timeout) {
	__ensure(!"pthread_rwlock_timedwrlock is not implemented");
}

EXPORT int pthread_rwlock_wrlock(pthread_rwlock_t* lock) {
	auto* ptr = reinterpret_cast<RwLock*>(lock);

	rwlock_mutex_lock(ptr, true);

	int rc_expected = ptr->writer_notify.load(hz::memory_order::acquire);
	while (true) {
		if (!rc_expected) {
			break;
		}

		__ensure(rc_expected & RC_COUNT_MASK);

		if (!(rc_expected & RC_WAITERS_BIT)) {
			if (!ptr->writer_notify.compare_exchange_weak(
				rc_expected,
				rc_expected | RC_COUNT_MASK,
				hz::memory_order::acquire,
				hz::memory_order::acquire
				)) {
				continue;
			}
		}

		sys_futex_wait(ptr->writer_notify.data(), rc_expected | RC_WAITERS_BIT, nullptr);
		rc_expected = ptr->writer_notify.load(hz::memory_order::acquire);
	}
	return 0;
}

EXPORT int pthread_rwlock_unlock(pthread_rwlock_t* lock) {
	auto* ptr = reinterpret_cast<RwLock*>(lock);
	auto rc_expected = ptr->writer_notify.load(hz::memory_order::relaxed);
	if (!rc_expected) {
		rwlock_mutex_unlock(ptr);
	}
	else {
		while (true) {
			auto count = rc_expected & RC_COUNT_MASK;
			__ensure(count);

			if (count == 1 && (rc_expected & RC_WAITERS_BIT)) {
				if (!ptr->writer_notify.compare_exchange_weak(
					rc_expected,
					0,
					hz::memory_order::release,
					hz::memory_order::relaxed
					)) {
					continue;
				}

				sys_futex_wake_all(ptr->writer_notify.data());
				break;
			}
			else {
				if (!ptr->writer_notify.compare_exchange_weak(
					rc_expected,
					(rc_expected & ~RC_COUNT_MASK) | (count - 1),
					hz::memory_order::release,
					hz::memory_order::relaxed
					)) {
					continue;
				}
				break;
			}
		}
	}
	return 0;
}

EXPORT int pthread_mutex_init(pthread_mutex_t* __restrict mutex, const pthread_mutexattr_t* __restrict attr) {
	return thread_mutex_init(mutex, attr);
}

EXPORT int pthread_mutex_destroy(pthread_mutex_t* mutex) {
	return thread_mutex_destroy(mutex);
}

EXPORT int pthread_mutex_lock(pthread_mutex_t* mutex) {
	return thread_mutex_lock(mutex);
}

EXPORT int pthread_mutex_trylock(pthread_mutex_t* mutex) {
	return thread_mutex_trylock(mutex);
}

EXPORT int pthread_mutex_timedlock(pthread_mutex_t* __restrict mutex, const timespec* __restrict abs_timeout) {
	__ensure(!"pthread_mutex_timedlock is not implemented");
}

EXPORT int pthread_mutex_unlock(pthread_mutex_t* mutex) {
	return thread_mutex_unlock(mutex);
}

EXPORT int pthread_once(pthread_once_t* once, void (*init_fn)()) {
	return thread_once(once, init_fn);
}

#define CPUCLOCK_SCHED 2
#define CPUCLOCK_PERTHREAD_MASK 4

EXPORT int pthread_getcpuclockid(pthread_t thread, clockid_t* clock_id) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	*clock_id = ~tcb->tid << 3 | CPUCLOCK_SCHED | CPUCLOCK_PERTHREAD_MASK;
	return 0;
}

struct Cond {
	hz::atomic<int> futex {};
	clockid_t clock {CLOCK_REALTIME};
};
static_assert(sizeof(Cond) <= sizeof(pthread_cond_t));

EXPORT int pthread_cond_init(pthread_cond_t* __restrict cond, const pthread_condattr_t* __restrict attr) {
	return thread_cond_init(cond, attr);
}

EXPORT int pthread_cond_destroy(pthread_cond_t* cond) {
	return thread_cond_destroy(cond);
}

EXPORT int pthread_cond_wait(pthread_cond_t* __restrict cond, pthread_mutex_t* __restrict mutex) {
	return thread_cond_wait(cond, mutex);
}

EXPORT int pthread_cond_timedwait(
	pthread_cond_t* __restrict cond,
	pthread_mutex_t* __restrict mutex,
	const timespec* __restrict abs_timeout) {
	return thread_cond_timedwait(cond, mutex, abs_timeout);
}

EXPORT int pthread_cond_clockwait(
	pthread_cond_t* __restrict cond,
	pthread_mutex_t* __restrict mutex,
	clockid_t clock_id,
	const struct timespec* __restrict abs_timeout) {
	if (!abs_timeout) {
		return thread_cond_wait(cond, mutex);
	}
	else {
		auto* ptr = reinterpret_cast<Cond*>(cond);

		while (true) {
			timespec current {};
			if (auto err = sys_clock_gettime(clock_id, &current)) {
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

EXPORT int pthread_cond_broadcast(pthread_cond_t* cond) {
	return thread_cond_broadcast(cond);
}

EXPORT int pthread_cond_signal(pthread_cond_t* cond) {
	return thread_cond_signal(cond);
}

namespace {
	struct Barrier {
		hz::atomic<int> waiting {};
		hz::atomic<int> inside {};
		hz::atomic<int> sequence {};
		int count {};
	};
	static_assert(sizeof(Barrier) <= sizeof(pthread_barrier_t));
}

EXPORT int pthread_barrier_init(
	pthread_barrier_t* __restrict barrier,
	const pthread_barrierattr_t* __restrict attr,
	unsigned int count) {
	new (barrier) Barrier {
		.count = static_cast<int>(count)
	};
	return 0;
}

EXPORT int pthread_barrier_destroy(pthread_barrier_t* __restrict barrier) {
	auto* ptr = reinterpret_cast<Barrier*>(barrier);
	int inside = 0;
	do {
		int expected = ptr->inside.load(hz::memory_order::relaxed);
		if (!expected) {
			break;
		}
		auto err = sys_futex_wait(ptr->inside.data(), expected, nullptr);
		__ensure(err == 0 || err == EAGAIN || err == EINTR);
	} while (inside);
	return 0;
}

EXPORT int pthread_barrier_wait(pthread_barrier_t* barrier) {
	auto* ptr = reinterpret_cast<Barrier*>(barrier);
	ptr->inside.fetch_add(1, hz::memory_order::acquire);

	auto leave = [&]() {
		int inside = ptr->inside.fetch_sub(1, hz::memory_order::release) - 1;
		if (inside == 0) {
			sys_futex_wake_all(ptr->inside.data());
		}
	};

	int sequence = ptr->sequence.load(hz::memory_order::acquire);
	while (true) {
		int expected = ptr->waiting.load(hz::memory_order::relaxed);
		if (ptr->waiting.compare_exchange_weak(
			expected,
			expected + 1,
			hz::memory_order::acquire,
			hz::memory_order::acquire)) {
			if (expected + 1 == ptr->count) {
				ptr->sequence.fetch_add(1, hz::memory_order::acquire);
				ptr->waiting.store(0, hz::memory_order::release);
				sys_futex_wake_all(ptr->sequence.data());
				leave();
				return PTHREAD_BARRIER_SERIAL_THREAD;
			}

			while (true) {
				auto err = sys_futex_wait(ptr->sequence.data(), sequence, nullptr);
				__ensure(err == 0 || err == EAGAIN || err == EINTR);
				int new_sequence = ptr->sequence.load(hz::memory_order::acquire);
				if (new_sequence > sequence) {
					leave();
					return 0;
				}
			}
		}
	}
}

EXPORT int pthread_attr_init(pthread_attr_t* attr) {
	auto* ptr = reinterpret_cast<ThreadAttr*>(attr);
	*ptr = {};
	return 0;
}

EXPORT int pthread_attr_destroy(pthread_attr_t* attr) {
	return 0;
}

EXPORT int pthread_attr_setdetachstate(pthread_attr_t* attr, int detach_state) {
	auto* ptr = reinterpret_cast<ThreadAttr*>(attr);
	ptr->detached = detach_state == PTHREAD_CREATE_DETACHED;
	return 0;
}

EXPORT int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stack_size) {
	if (stack_size < PTHREAD_STACK_MIN) {
		return EINVAL;
	}

	auto* ptr = reinterpret_cast<ThreadAttr*>(attr);
	ptr->stack_size = stack_size;
	return 0;
}

EXPORT int pthread_attr_setschedpolicy(pthread_attr_t* __restrict attr, int policy) {
	auto* ptr = reinterpret_cast<ThreadAttr*>(attr);
	ptr->policy = policy;
	return 0;
}

EXPORT int pthread_attr_setschedparam(pthread_attr_t* __restrict attr, const sched_param* __restrict param) {
	auto* ptr = reinterpret_cast<ThreadAttr*>(attr);
	ptr->priority = param->sched_priority;
	return 0;
}

EXPORT int pthread_attr_setinheritsched(pthread_attr_t* attr, int inherit_sched) {
	if (inherit_sched != PTHREAD_INHERIT_SCHED && inherit_sched != PTHREAD_EXPLICIT_SCHED) {
		return EINVAL;
	}

	auto* ptr = reinterpret_cast<ThreadAttr*>(attr);
	ptr->inherit_sched = inherit_sched;
	return 0;
}

EXPORT int pthread_mutexattr_init(pthread_mutexattr_t* attr) {
	auto* ptr = reinterpret_cast<MutexAttr*>(attr);
	*ptr = {};
	return 0;
}

EXPORT int pthread_mutexattr_destroy(pthread_mutexattr_t* attr) {
	return 0;
}

EXPORT int pthread_mutexattr_gettype(const pthread_mutexattr_t* __restrict attr, int* __restrict type) {
	auto* ptr = reinterpret_cast<const MutexAttr*>(attr);
	return ptr->type;
}

EXPORT int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type) {
	auto* ptr = reinterpret_cast<MutexAttr*>(attr);
	ptr->type = type;
	return 0;
}

EXPORT int pthread_mutexattr_setprotocol(pthread_mutexattr_t* __attr, int protocol) {
	return 0;
}

EXPORT int pthread_condattr_init(pthread_condattr_t* attr) {
	new (attr) CondAttr {};
	return 0;
}

EXPORT int pthread_condattr_destroy(pthread_condattr_t* attr) {
	return 0;
}

EXPORT int pthread_condattr_setclock(pthread_condattr_t* attr, clockid_t clock_id) {
	reinterpret_cast<CondAttr*>(attr)->clock = clock_id;
	return 0;
}

EXPORT int pthread_sigmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old) {
	if (auto err = sys_sigprocmask(how, set, old)) {
		return err;
	}
	return 0;
}

ALIAS(pthread_key_create, __pthread_key_create);
