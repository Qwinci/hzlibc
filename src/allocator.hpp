#pragma once
#include <stddef.h>
#include "utils.hpp"

void* allocate(size_t size);
void* reallocate(void* old, size_t new_size);
void deallocate(void* ptr);

struct Allocator {
	static void* allocate(size_t size) {
		auto ret = ::allocate(size);
		__ensure(ret);
		return ret;
	}

	static void deallocate(void* ptr) {
		return ::deallocate(ptr);
	}
};
