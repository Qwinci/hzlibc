#include "stdlib.h"
#include "utils.hpp"
#include "errno.h"

EXPORT void* reallocarray(void* old, size_t num_blocks, size_t size) {
	size_t total = num_blocks * size;
	if (total / num_blocks != size) {
		errno = EINVAL;
		return nullptr;
	}
	return realloc(old, total);
}
