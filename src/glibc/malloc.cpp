#include "malloc.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "stdlib.h"
#include "errno.h"

EXPORT int malloc_trim(size_t) {
	return 0;
}

EXPORT struct mallinfo mallinfo() {
	return {};
}

EXPORT struct mallinfo2 mallinfo2() {
	return {};
}

EXPORT size_t malloc_usable_size(void* ptr) {
	return size_for_allocation(ptr);
}

EXPORT void* memalign(size_t alignment, size_t size) {
	void* ptr;
	int err = posix_memalign(&ptr, alignment, size);
	if (err != 0) {
		errno = err;
		return nullptr;
	}
	return ptr;
}
