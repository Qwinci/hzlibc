#include "utils.hpp"

#define memcpy __builtin_memcpy

EXPORT uintptr_t __stack_chk_guard;

void init_stack_guard(void* entropy) {
	if (entropy) {
		memcpy(&__stack_chk_guard, entropy, sizeof(uintptr_t));
	}
	else {
		__stack_chk_guard = '\n' << 16 | 255 << 24;
	}
}

extern "C" EXPORT void __stack_chk_fail() {
	panic("stack smashing detected");
}
