#include "pthread.h"
#include "signal.h"
#include "utils.hpp"
#include "sys.hpp"
#include "internal/tcb.hpp"
#include "rtld/rtld.hpp"
#include "errno.h"
#include "mutex.hpp"
#include <hz/array.hpp>
#include <hz/new.hpp>
#include <limits.h>

EXPORT pthread_t pthread_self() {
	return reinterpret_cast<pthread_t>(get_current_tcb());
}

EXPORT int pthread_equal(pthread_t t1, pthread_t t2) {
	return t1 == t2;
}

namespace {
	struct Attr {
		size_t stack_size;
		bool detached;
	};
	static_assert(sizeof(Attr) <= sizeof(pthread_attr_t));
}

EXPORT int pthread_create(
	pthread_t* __restrict thread,
	const pthread_attr_t* __restrict attr,
	void* (*start_fn)(void* arg),
	void* __restrict arg) {
	void* tcb;
	void* tp;
	if (!__dlapi_create_tcb(&tcb, &tp)) {
		return ENOMEM;
	}

	void* stack_base = nullptr;
	size_t stack_size = 0x200000;

	if (attr) {
		auto* ptr = reinterpret_cast<const Attr*>(attr);
		if (ptr->stack_size) {
			stack_size = ptr->stack_size;
		}
		if (ptr->detached) {
			static_cast<Tcb*>(tcb)->detached = true;
		}
	}

	pid_t tid;
	if (auto err = sys_thread_create(stack_base, stack_size, start_fn, arg, tp, &tid)) {
		__dlapi_destroy_tcb(tcb);
		return err;
	}

	*thread = reinterpret_cast<pthread_t>(tcb);

	auto* tcb_ptr = static_cast<Tcb*>(tcb);
	__atomic_store_n(&tcb_ptr->tid, tid, __ATOMIC_RELAXED);
	sys_futex_wake(&tcb_ptr->tid);

	return 0;
}

EXPORT int pthread_join(pthread_t thread, void** ret) {
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

EXPORT int pthread_detach(pthread_t thread) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	tcb->detached = true;
	return 0;
}

EXPORT int pthread_setcanceltype(int state, int* old_state) {
	println("pthread_setcanceltype: pthread cancellation is not implemented");
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

namespace {
	struct Key {
		void (*destructor)(void* arg) {};
		int generation {};
		bool used {};
	};

	hz::array<Key, PTHREAD_KEYS_MAX> KEYS {};
	Mutex KEYS_MUTEX {};
}

void pthread_call_destructors() {
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

EXPORT int pthread_key_create(pthread_key_t* key, void (*destructor)(void* arg)) {
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

EXPORT int pthread_key_delete(pthread_key_t key) {
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

EXPORT void* pthread_getspecific(pthread_key_t key) {
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

EXPORT int pthread_setspecific(pthread_key_t key, const void* value) {
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
#ifdef __x86_64__
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

EXPORT int pthread_rwlock_rdlock(pthread_rwlock_t* lock) {
	auto* ptr = reinterpret_cast<RwLock*>(lock);

	rwlock_mutex_lock(ptr, false);
	ptr->writer_notify.fetch_add(1, hz::memory_order::acquire);
	rwlock_mutex_unlock(ptr);

	return 0;
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

namespace {
	struct MutexAttr {
		int type : 4;
	};
	static_assert(sizeof(MutexAttr) <= sizeof(pthread_mutexattr_t));
}

EXPORT int pthread_mutex_init(pthread_mutex_t* __restrict mutex, const pthread_mutexattr_t* __restrict attr) {
	auto* ptr = reinterpret_cast<Mutex*>(mutex);
	new (ptr) Mutex {};

	if (attr) {
		auto* attr_ptr = reinterpret_cast<const MutexAttr*>(attr);
		ptr->state.kind = attr_ptr->type;
	}
	return 0;
}

EXPORT int pthread_mutex_destroy(pthread_mutex_t* mutex) {
	return 0;
}

EXPORT int pthread_mutex_lock(pthread_mutex_t* mutex) {
	return reinterpret_cast<Mutex*>(mutex)->manual_lock();
}

EXPORT int pthread_mutex_trylock(pthread_mutex_t* mutex) {
	return reinterpret_cast<Mutex*>(mutex)->try_manual_lock();
}

EXPORT int pthread_mutex_unlock(pthread_mutex_t* mutex) {
	return reinterpret_cast<Mutex*>(mutex)->manual_unlock();
}

namespace {
	constexpr int ONCE_INCOMPLETE = 0;
	constexpr int ONCE_QUEUED = 1;
	constexpr int ONCE_RUNNING = 2;
	constexpr int ONCE_COMPLETE = 3;
}

EXPORT int pthread_once(pthread_once_t* once, void (*init_fn)()) {
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

struct CondAttr {
	clockid_t clock;
};
static_assert(sizeof(CondAttr) <= sizeof(pthread_condattr_t));

EXPORT int pthread_cond_init(pthread_cond_t* __restrict cond, const pthread_condattr_t* __restrict attr) {
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

EXPORT int pthread_cond_destroy(pthread_cond_t*) {
	return 0;
}

EXPORT int pthread_cond_wait(pthread_cond_t* __restrict cond, pthread_mutex_t* __restrict mutex) {
	auto* ptr = reinterpret_cast<Cond*>(cond);
	auto value = ptr->futex.load(hz::memory_order::relaxed);
	pthread_mutex_unlock(mutex);
	sys_futex_wait(ptr->futex.data(), value, nullptr);
	pthread_mutex_lock(mutex);
	return 0;
}

EXPORT int pthread_cond_timedwait(
	pthread_cond_t* __restrict cond,
	pthread_mutex_t* __restrict mutex,
	const struct timespec* __restrict abs_time) {
	if (!abs_time) {
		return pthread_cond_wait(cond, mutex);
	}
	else {
		auto* ptr = reinterpret_cast<Cond*>(cond);

		while (true) {
			timespec current {};
			if (clock_gettime(ptr->clock, &current)) {
				return errno;
			}

			timespec timeout {
				.tv_sec = abs_time->tv_sec - current.tv_sec,
				.tv_nsec = abs_time->tv_nsec - current.tv_nsec
			};
			if (timeout.tv_sec < 0 || (timeout.tv_sec == 0 && timeout.tv_nsec < 0)) {
				return ETIMEDOUT;
			}

			auto value = ptr->futex.load(hz::memory_order::relaxed);
			pthread_mutex_unlock(mutex);
			int err = sys_futex_wait(ptr->futex.data(), value, &timeout);
			pthread_mutex_lock(mutex);
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
	auto* ptr = reinterpret_cast<Cond*>(cond);
	ptr->futex.fetch_add(1, hz::memory_order::relaxed);
	sys_futex_wake_all(ptr->futex.data());
	return 0;
}

EXPORT int pthread_cond_signal(pthread_cond_t* cond) {
	auto* ptr = reinterpret_cast<Cond*>(cond);
	ptr->futex.fetch_add(1, hz::memory_order::relaxed);
	sys_futex_wake(ptr->futex.data());
	return 0;
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
			sys_futex_wake(ptr->inside.data());
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
				sys_futex_wake(ptr->sequence.data());
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
	auto* ptr = reinterpret_cast<Attr*>(attr);
	*ptr = {};
	return 0;
}

EXPORT int pthread_attr_destroy(pthread_attr_t* attr) {
	return 0;
}

EXPORT int pthread_attr_setdetachstate(pthread_attr_t* attr, int detach_state) {
	auto* ptr = reinterpret_cast<Attr*>(attr);
	ptr->detached = detach_state == PTHREAD_CREATE_DETACHED;
	return 0;
}

EXPORT int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stack_size) {
	if (stack_size < PTHREAD_STACK_MIN) {
		return EINVAL;
	}

	auto* ptr = reinterpret_cast<Attr*>(attr);
	ptr->stack_size = stack_size;
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
