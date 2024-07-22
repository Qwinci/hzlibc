#include "string.h"
#include "utils.hpp"

EXPORT void* rawmemchr(const void* ptr, int ch) {
	return memchr(ptr, ch, SIZE_MAX);
}

EXPORT void* mempcpy(void* __restrict dest, const void* __restrict src, size_t size) {
	memcpy(dest, src, size);
	return reinterpret_cast<unsigned char*>(dest) + size;
}

EXPORT void* memrchr(const void* ptr, int ch, size_t count) {
	auto* p = static_cast<const unsigned char*>(ptr) + count - 1;
	for (; count; --p, --count) {
		if (*p == static_cast<unsigned char>(ch)) {
			return const_cast<unsigned char*>(p);
		}
	}
	return nullptr;
}
