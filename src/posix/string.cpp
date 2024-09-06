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

EXPORT char* strtok_r(char* __restrict str, const char* delim, char** __restrict save_ptr) {
	if (str) {
		*save_ptr = str;
	}

	hz::string_view str_view {*save_ptr};
	hz::string_view delim_str {delim};

	auto* start = *save_ptr;

	auto start_pos = str_view.find_first_not_of(delim_str);
	if (start_pos == hz::string_view::npos) {
		return nullptr;
	}

	auto end_pos = str_view.find_first_of(delim_str, start_pos);
	if (end_pos != hz::string_view::npos) {
		(*save_ptr)[end_pos] = 0;
		*save_ptr += end_pos + 1;
	}
	else {
		*save_ptr += end_pos;
	}
	return start + start_pos;
}

EXPORT int strcoll_l(const char* lhs, const char* rhs, locale_t locale) {
	println("strcoll_l ignores locale");
	return strcoll(lhs, rhs);
}

EXPORT size_t strxfrm_l(char* __restrict dest, const char* __restrict src, size_t count, locale_t locale) {
	//println("strxfrm_l ignores locale");
	return strxfrm(dest, src, count);
}

EXPORT char* strerror_l(int err_num, locale_t locale) {
	println("strerror_l ignores locale");
	return strerror(err_num);
}

EXPORT char* strsignal(int sig) {
	__ensure(!"strsignal is not implemented");
}

ALIAS(strtok_r, __strtok_r);
ALIAS(strcoll_l, __strcoll_l);
ALIAS(strxfrm_l, __strxfrm_l);
