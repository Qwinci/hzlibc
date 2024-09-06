#include "malloc.h"
#include "utils.hpp"
#include "allocator.hpp"

EXPORT int malloc_trim(size_t) {
	return 0;
}

EXPORT struct mallinfo2 mallinfo2() {
	return {};
}

EXPORT size_t malloc_usable_size(void* ptr) {
	return size_for_allocation(ptr);
}
