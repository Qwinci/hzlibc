#pragma once
#include <hz/atomic.hpp>

static_assert(sizeof(hz::atomic<int>) == 4);

#ifdef __x86_64__

struct pthread_mutex_internal {
	hz::atomic<int> state;
	int recursion;
	char unused[8];
	int kind;
};

#elif defined(__i386__)

struct pthread_mutex_internal {
	hz::atomic<int> state;
	int recursion;
	char unused[4];
	int kind;
};

#else

#error missing architecture specific code

#endif
