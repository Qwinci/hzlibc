#pragma once
#include "ctype.h"
#include "wctype.h"
#include "errno.h"
#include "wchar.h"
#include <hz/limits.hpp>

namespace {
	template<typename C>
	inline int isspace_helper(C c) {
		if constexpr (hz::is_same_v<C, char>) {
			return isspace(c);
		}
		else {
			return iswspace(c);
		}
	}

	template<typename C>
	inline C tolower_helper(C c) {
		if constexpr (hz::is_same_v<C, char>) {
			return tolower(c);
		}
		else {
			return towlower(c);
		}
	}
}

template<typename T, typename U, typename C = char>
T str_to_int(const C* __restrict ptr, C** __restrict end_ptr, int base) {
	while (isspace_helper(*ptr)) {
		++ptr;
	}

	bool sign = false;
	if (*ptr == '-') {
		++ptr;
		sign = true;
	}
	else if (*ptr == '+') {
		++ptr;
	}

	auto valid_char = [&](C c) {
		if (base <= 10) {
			return c >= '0' && c <= '0' + base - 1;
		}
		else {
			return (c >= '0' && c <= '9') || ((c | 1 << 5) >= 'a' && (c | 1 << 5) <= 'a' + (base - 10));
		}
	};

	if (base == 0) {
		if (ptr[0] == '0' && tolower_helper(ptr[1]) == 'x' && (valid_char(ptr[2]))) {
			base = 16;
			ptr += 2;
		}
		else if (ptr[0] == '0') {
			base = 8;
			++ptr;
		}
		else {
			base = 10;
		}
	}
	else if (base == 2) {
		if (ptr[0] == '0' && tolower_helper(ptr[1]) == 'b' && ptr[2]) {
			ptr += 2;
		}
	}
	else if (base == 8) {
		if (*ptr == '0') {
			++ptr;
		}
	}
	else if (base == 16) {
		if (ptr[0] == '0' && tolower_helper(ptr[1]) == 'x' && ptr[2]) {
			ptr += 2;
		}
	}

	U max_value;
	if constexpr (hz::is_signed_v<T>) {
		if (sign) {
			max_value = static_cast<U>(-(hz::numeric_limits<T>::min() + 1)) + 1;
		}
		else {
			max_value = static_cast<U>(hz::numeric_limits<T>::max());
		}
	}
	else {
		max_value = static_cast<U>(hz::numeric_limits<T>::max());
	}

	U value = 0;
	bool overflow = false;
	if (base <= 36) {
		for (; valid_char(*ptr); ++ptr) {
			auto old = value;
			value *= base;
			if (value / base != old || value > max_value) {
				overflow = true;
			}
			old = value;
			value += *ptr <= '9' ? (*ptr - '0') : (tolower_helper(*ptr) - 'a' + 10);
			if (value < old || value > max_value) {
				overflow = true;
			}
		}
	}

	if (end_ptr) {
		*end_ptr = const_cast<C*>(ptr);
	}

	if (overflow) {
		errno = ERANGE;
		return sign ? hz::numeric_limits<T>::min() : hz::numeric_limits<T>::max();
	}

	if (sign) {
		return -static_cast<T>(value);
	}
	else {
		return static_cast<T>(value);
	}
}
