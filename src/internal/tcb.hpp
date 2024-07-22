#pragma once
#include <stdint.h>
#include <hz/atomic.hpp>

struct Tcb {
	Tcb* self {this};
#ifdef __x86_64__
	uint8_t pad[0x20] {};
#elif defined(__i386__)
	uint8_t pad[0x10] {};
#endif
	uintptr_t stack_canary {0xCAFEBABE};
	int tid {};
	hz::atomic<int> exited {};
	void* exit_status {};
};
#ifdef __x86_64__
static_assert(offsetof(Tcb, stack_canary) == 0x28);
#elif defined(__i386__)
static_assert(offsetof(Tcb, stack_canary) == 0x14);
#endif


inline Tcb* get_current_tcb() {
	Tcb* tcb;
#ifdef __x86_64__
	asm volatile("mov %%fs:0, %0" : "=r"(tcb));
#elif defined(__i386__)
	asm volatile("mov %%gs:0, %0" : "=r"(tcb));
#else
#error missing architecture specific code
#endif
	return tcb;
}
