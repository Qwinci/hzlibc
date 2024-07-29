#pragma once
#include "glibc/pthread_internal.hpp"
#include "utils.hpp"

struct Mutex {
	constexpr explicit Mutex(int kind = 0) {
		state.kind = kind;
	}

	int manual_lock();
	int try_manual_lock();
	int manual_unlock();

	struct Guard {
		inline ~Guard() {
			__ensure(owner->manual_unlock() == 0);
		}

	private:
		friend Mutex;
		constexpr explicit Guard(Mutex* owner) : owner {owner} {}

		Mutex* owner;
	};

	[[nodiscard]] inline Guard lock() {
		__ensure(manual_lock() == 0);
		return Guard {this};
	}

	pthread_mutex_internal state {};
};
