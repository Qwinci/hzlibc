#pragma once
#include "glibc/pthread_internal.hpp"
#include "utils.hpp"

struct Mutex {
	int manual_lock();
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

	inline Guard lock() {
		__ensure(manual_lock() == 0);
		return Guard {this};
	}

private:
	pthread_mutex_internal state;
};
