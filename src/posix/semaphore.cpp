#include "semaphore.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include <hz/atomic.hpp>
#include <hz/new.hpp>

namespace {
	struct Semaphore {
		hz::atomic<int> state;
		bool pshared;
	};
	static_assert(sizeof(Semaphore) <= sizeof(sem_t));

	constexpr int SEMAPHORE_COUNT_MASK = SEM_VALUE_MAX;
	constexpr int SEMAPHORE_WAITERS = 1 << 31;
}

EXPORT int sem_init(sem_t* sem, int pshared, unsigned int value) {
	if (value > SEM_VALUE_MAX) {
		errno = EINVAL;
		return -1;
	}
	new (sem) Semaphore {
		.state {static_cast<int>(value)},
		.pshared = pshared != 0
	};
	return 0;
}

EXPORT int sem_destroy(sem_t* sem) {
	return 0;
}

EXPORT int sem_post(sem_t* sem) {
	auto* ptr = reinterpret_cast<Semaphore*>(sem);
	int count = ptr->state.load(hz::memory_order::relaxed) & SEMAPHORE_COUNT_MASK;
	if (count == SEM_VALUE_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	auto state = ptr->state.exchange(count + 1, hz::memory_order::release);
	if (state & SEMAPHORE_WAITERS) {
		__ensure(sys_futex_wake(ptr->state.data(), ptr->pshared) == 0);
	}
	return 0;
}

EXPORT int sem_wait(sem_t* sem) {
	auto* ptr = reinterpret_cast<Semaphore*>(sem);
	int state = 0;
	while (true) {
		if (ptr->state.compare_exchange_weak(
			state,
			SEMAPHORE_WAITERS,
			hz::memory_order::acquire)) {
			int err = sys_futex_wait(ptr->state.data(), state, nullptr, ptr->pshared);

			if (err == ETIMEDOUT || err == EINTR) {
				errno = err;
				return -1;
			}
			else {
				__ensure(!err || err == EAGAIN);
				continue;
			}
		}
		else {
			if (ptr->state.compare_exchange_strong(
				state,
				state - 1,
				hz::memory_order::relaxed,
				hz::memory_order::relaxed)) {
				return 0;
			}
		}
	}
}

EXPORT int sem_trywait(sem_t* sem) {
	auto* ptr = reinterpret_cast<Semaphore*>(sem);
	while (true) {
		auto state = ptr->state.load(hz::memory_order::acquire);
		if (state & SEMAPHORE_WAITERS) {
			errno = EAGAIN;
			return -1;
		}
		if (ptr->state.compare_exchange_weak(
			state,
			state - 1,
			hz::memory_order::release,
			hz::memory_order::relaxed
			)) {
			return 0;
		}
	}
}

EXPORT int sem_timedwait(sem_t* __restrict sem, const struct timespec* __restrict abs_timeout) {
	auto* ptr = reinterpret_cast<Semaphore*>(sem);
	int state = 0;
	while (true) {
		if (ptr->state.compare_exchange_weak(
			state,
			SEMAPHORE_WAITERS,
			hz::memory_order::acquire)) {
			int err;
			if (abs_timeout) {
				timespec current {};
				if (clock_gettime(CLOCK_REALTIME, &current)) {
					return -1;
				}

				timespec timeout {
					.tv_sec = abs_timeout->tv_sec - current.tv_sec,
					.tv_nsec = abs_timeout->tv_nsec - current.tv_nsec
				};
				if (timeout.tv_sec < 0 || (timeout.tv_sec == 0 && timeout.tv_nsec < 0)) {
					errno = ETIMEDOUT;
					return -1;
				}

				err = sys_futex_wait(ptr->state.data(), state, &timeout, ptr->pshared);
			}
			else {
				err = sys_futex_wait(ptr->state.data(), state, nullptr, ptr->pshared);
			}

			if (err == ETIMEDOUT || err == EINTR) {
				errno = err;
				return -1;
			}
			else {
				__ensure(!err || err == EAGAIN);
				continue;
			}
		}
		else {
			if (ptr->state.compare_exchange_strong(
				state,
				state - 1,
				hz::memory_order::relaxed,
				hz::memory_order::relaxed)) {
				return 0;
			}
		}
	}
}
