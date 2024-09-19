#include "string.h"
#include "utils.hpp"
#include "ctype.h"
#include "strings.h"

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

EXPORT void* memmem(const void* ptr, size_t len, const void* search, size_t search_len) {
	hz::string_view a {static_cast<const char*>(ptr), len};
	hz::string_view b {static_cast<const char*>(search), search_len};
	if (auto pos = a.find(b); pos != hz::string_view::npos) {
		return const_cast<char*>(static_cast<const char*>(ptr) + pos);
	}
	return nullptr;
}

EXPORT char* strerror_r(int err_num, char*, size_t) {
	return const_cast<char*>(strerror(err_num));
}

EXPORT int strverscmp(const char* s1, const char* s2) {
	size_t seq_pos = 0;
	bool is_zero = true;
	size_t i = 0;
	for (; s1[i] == s2[i]; ++i) {
		if (!s1[i]) {
			return 0;
		}
		else if (!isdigit(s1[i])) {
			seq_pos = i + 1;
			is_zero = true;
		}
		else if (s1[i] != '0') {
			is_zero = false;
		}
	}

	if (s1[seq_pos] != '0' && s2[seq_pos] != '0') {
		size_t j = i;
		for (; isdigit(s1[j]); ++j) {
			if (!isdigit(s2[j])) {
				return 1;
			}
		}
		if (isdigit(s2[j])) {
			return -1;
		}
	}
	else if (is_zero && seq_pos < i && (isdigit(s1[i]) || isdigit(s2[i]))) {
		return (s1[i] - '0') - (s2[i] - '0');
	}

	return s1[i] - s2[i];
}

EXPORT char* strcasestr(const char* str, const char* substr) {
	auto len = strlen(substr);
	for (; *str; ++str) {
		if (strncasecmp(str, substr, len) == 0) {
			return const_cast<char*>(str);
		}
	}
	return nullptr;
}

EXPORT char* strchrnul(const char* str, int ch) {
	for (; *str; ++str) {
		if (*str == ch) {
			return const_cast<char*>(str);
		}
	}
	return const_cast<char*>(str);
}

EXPORT char* strsep(char** __restrict str, const char* __restrict delim) {
	char* token = *str;
	if (!token) {
		return nullptr;
	}

	auto ptr = token;
	while (*ptr && !strchr(delim, *ptr)) {
		++ptr;
	}

	if (*ptr) {
		*ptr = 0;
		*str = ptr + 1;
	}
	else {
		*str = nullptr;
	}

	return token;
}

ALIAS(rawmemchr, __rawmemchr);
