#include "strings.h"
#include "utils.hpp"
#include "ctype.h"

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
