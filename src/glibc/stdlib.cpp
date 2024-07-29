#include "stdlib.h"
#include "utils.hpp"
#include "errno.h"
#include "stdio.h"
#include "fcntl.h"
#include "errno.h"
#include "unistd.h"

EXPORT void* reallocarray(void* old, size_t num_blocks, size_t size) {
	size_t total = num_blocks * size;
	if (total / num_blocks != size) {
		errno = EINVAL;
		return nullptr;
	}
	return realloc(old, total);
}

EXPORT char* secure_getenv(const char* name) {
	// todo make this secure
	return getenv(name);
}

EXPORT int mkostemp(char* template_str, int flags) {
	hz::string_view str {template_str};
	if (!str.ends_with("XXXXXX")) {
		errno = EINVAL;
		return -1;
	}

	flags &= ~O_WRONLY;

	for (size_t i = 0; i < 999999; ++i) {
		__ensure(sprintf(template_str + str.size() - 6, "%06zu", i) == 6);

		int fd = open(template_str, O_RDWR | O_CREAT | O_EXCL | flags, 0600);
		if (fd < 0) {
			if (errno == EEXIST) {
				close(fd);
				continue;
			}
			return -1;
		}
		return fd;
	}

	errno = EEXIST;
	return -1;
}

EXPORT int initstate_r(
	unsigned int seed,
	char* __restrict state_buf,
	size_t state_len,
	random_data* __restrict buf) {
	//__ensure(!"initstate_r is not implemented");
	// todo
	println("initstate_r is not implemented");
	return 0;
}

#include "bsd/stdlib.h"

EXPORT int random_r(random_data* __restrict buf, int32_t* __restrict result) {
	//__ensure(!"random_r is not implemented");
	//println("random_r is not implemented");
	arc4random_buf(result, 4);
	return 0;
}

EXPORT void qsort_r(
	void* ptr,
	size_t count,
	size_t size,
	int (*comp)(const void* a, const void* b, void* arg),
	void* arg) {
	for (size_t i = 0; i < count; ++i) {
		bool changed = false;
		for (size_t j = 0; j < count - 1; ++j) {
			auto* a = reinterpret_cast<char*>(ptr) + j * size;
			auto* b = reinterpret_cast<char*>(ptr) + (j + 1) * size;
			if (comp(a, b, arg) > 0) {
				for (size_t k = 0; k < size; ++k) {
					char tmp = a[k];
					a[k] = b[k];
					b[k] = tmp;
				}
				changed = true;
			}
		}

		if (!changed) {
			break;
		}
	}
}

EXPORT int strfromf128(
	char* __restrict buf,
	size_t buf_size,
	const char* __restrict fmt,
	__float128 value) {
	__ensure(!"strfromf128 is not implemented");
}

EXPORT __float128 strtof128(const char* __restrict str, char** __restrict end_ptr) {
	__ensure(!"strtof128 is not implemented");
}
