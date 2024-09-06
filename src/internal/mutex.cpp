#include "mutex.hpp"
#include "sys.hpp"
#include "utils.hpp"
#include "errno.h"
#include "pthread.h"

namespace {
	constexpr int FLAG_WAITING = 1 << 31;
}

int Mutex::manual_lock() {
	auto id = sys_get_thread_id();

	int expected = 0;
	while (true) {
		if (!expected) {
			if (state.state.compare_exchange_strong(expected, id, hz::memory_order::acquire)) {
				state.recursion = 1;
				return 0;
			}
		}

		if ((expected & ~FLAG_WAITING) == id) {
			if (state.kind == PTHREAD_MUTEX_RECURSIVE) {
				++state.recursion;
				return 0;
			}
			else if (state.kind == PTHREAD_MUTEX_ERRORCHECK) {
				return EDEADLK;
			}
			else {
				panic("tried to lock a normal mutex multiple times");
			}
		}

		if (expected & FLAG_WAITING) {
			auto status = sys_futex_wait(state.state.data(), expected, nullptr);
			__ensure(status == 0 || status == EAGAIN || status == EINTR);
			expected = 0;
		}
		else {
			if (state.state.compare_exchange_weak(expected, expected | FLAG_WAITING, hz::memory_order::relaxed)) {
				expected |= FLAG_WAITING;
			}
		}
	}
}

int Mutex::try_manual_lock() {
	auto id = sys_get_thread_id();

	int expected = 0;
	if (state.state.compare_exchange_strong(expected, id, hz::memory_order::acquire)) {
		state.recursion = 1;
		return 0;
	}

	if ((expected & ~FLAG_WAITING) == id) {
		if (state.kind == PTHREAD_MUTEX_RECURSIVE) {
			++state.recursion;
			return 0;
		}
		else if (state.kind == PTHREAD_MUTEX_ERRORCHECK) {
			return EDEADLK;
		}
		else {
			panic("tried to lock a normal mutex multiple times");
		}
	}

	return EBUSY;
}

int Mutex::manual_unlock() {
	auto id = sys_get_thread_id();

	auto old_state = state.state.load(hz::memory_order::relaxed);
	if ((old_state & ~FLAG_WAITING) != id) {
		if (state.kind == PTHREAD_MUTEX_NORMAL) {
			panic("tried to unlock a normal mutex not owned by thread");
		}
		else {
			return EPERM;
		}
	}
	else if (!old_state) {
		if (state.kind == PTHREAD_MUTEX_NORMAL) {
			panic("tried to unlock an unlocked normal mutex");
		}
		else {
			return EPERM;
		}
	}

	__ensure(state.recursion >= 1);
	if (--state.recursion > 0) {
		return 0;
	}

	old_state = state.state.exchange(0, hz::memory_order::release);
	if (old_state & FLAG_WAITING) {
		auto status = sys_futex_wake_all(state.state.data());
		__ensure(status == 0 || status == EACCES || status == EINVAL);
	}
	return 0;
}
