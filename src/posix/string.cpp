#include "string.h"
#include "utils.hpp"

EXPORT size_t strnlen(const char* str, size_t max_len) {
	size_t len = 0;
	for (; max_len && *str; ++str, --max_len) ++len;
	return len;
}

EXPORT char* stpcpy(char* __restrict dest, const char* src) {
	for (; *src;) {
		*dest++ = *src++;
	}
	*dest = 0;
	return dest;
}
