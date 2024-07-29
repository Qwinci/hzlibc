#include "malloc.h"
#include "utils.hpp"

EXPORT int malloc_trim(size_t) {
	return 0;
}

EXPORT struct mallinfo2 mallinfo2() {
	return {};
}
