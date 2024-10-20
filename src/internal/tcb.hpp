#pragma once
#include "limits.h"
#include "allocator.hpp"
#include <stdint.h>
#include <hz/atomic.hpp>
#include <hz/vector.hpp>

struct [[gnu::aligned(16)]] Tcb {
	Tcb* self {this};
#if defined(__x86_64__) || defined(__aarch64__)
	uint8_t pad[0x20] {};
#elif defined(__i386__)
	uint8_t pad[0x10] {};
#else
#error missing architecture specific code
#endif
	uintptr_t stack_canary {0xCAFEBABE};
#if !ANSI_ONLY
	int tid {};
#endif
	hz::atomic<int> exited {};
	void* exit_status {};
	hz::vector<void*, Allocator> dtv {Allocator {}};

	struct Key {
		int generation;
		void* value;
	};
	Key keys[PTHREAD_KEYS_MAX] {};
	bool detached {};
};
#ifdef __x86_64__
static_assert(offsetof(Tcb, stack_canary) == 0x28);
static_assert(offsetof(Tcb, dtv) == 64);
#elif defined(__i386__)
static_assert(offsetof(Tcb, stack_canary) == 0x14);
#elif defined(__aarch64__)
static_assert(sizeof(Tcb) - 0x10 - offsetof(Tcb, dtv) == 16416);
static_assert(sizeof(Tcb) % 16 == 0);
#endif


inline Tcb* get_current_tcb() {
	Tcb* tcb;
#ifdef __x86_64__
	asm volatile("mov %%fs:0, %0" : "=r"(tcb));
#elif defined(__i386__)
	asm volatile("mov %%gs:0, %0" : "=r"(tcb));
#elif defined(__aarch64__)
	uint64_t tp;
	asm volatile("mrs %0, tpidr_el0" : "=r"(tp));
	tcb = reinterpret_cast<Tcb*>(tp + 0x10 - sizeof(Tcb));
#else
#error missing architecture specific code
#endif
	return tcb;
}
