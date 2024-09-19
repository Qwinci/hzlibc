#include "strings.h"
#include "utils.hpp"
#include "ctype.h"
#include "string.h"

EXPORT int strcasecmp(const char* s1, const char* s2) {
	for (;; ++s1, ++s2) {
		int res = tolower(*s1) - tolower(*s2);
		if (res != 0 || !*s1) {
			return res;
		}
	}
}

EXPORT int strncasecmp(const char* s1, const char* s2, size_t count) {
	for (; count; --count, ++s1, ++s2) {
		int res = tolower(*s1) - tolower(*s2);
		if (res != 0 || !*s1) {
			return res;
		}
	}
	return 0;
}

EXPORT int bcmp(const void* s1, const void* s2, size_t size) {
	return memcmp(s1, s2, size);
}

EXPORT void bcopy(const void* src, void* dest, size_t size) {
	memmove(dest, src, size);
}

EXPORT int ffs(int value) {
	for (size_t i = 0; i < sizeof(int) * 8; ++i) {
		if (value & 1 << i) {
			return static_cast<int>(i + 1);
		}
	}
	return 0;
}
