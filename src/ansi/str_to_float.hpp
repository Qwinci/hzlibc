#pragma once
#include "ctype.h"
#include "wctype.h"
#include "errno.h"
#include "wchar.h"
#include "math.h"
#include "str_to_int.hpp"
#include <hz/limits.hpp>
#include <hz/type_traits.hpp>

namespace {
	template<typename C>
	int str_case_cmp(const C* s1, const char* s2, size_t count) {
		for (; count; --count, ++s1, ++s2) {
			int res = tolower_helper(*s1) - tolower_helper(*s2);
			if (res != 0 || !*s1) {
				return res;
			}
		}
		return 0;
	}
}

template<typename T, typename C>
T str_to_float(const C* __restrict ptr, C** __restrict end_ptr) {
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

	if (str_case_cmp(ptr, "nan", 3) == 0) {
		ptr += 3;
		if (*ptr == '(') {
			while (*ptr != ')') {
				++ptr;
			}
			++ptr;
		}

		if (end_ptr) {
			*end_ptr = const_cast<C*>(ptr);
		}

		if constexpr (sizeof(T) == 4) {
			return __builtin_nanf("");
		}
		else if constexpr (sizeof(T) == 8) {
			return __builtin_nan("");
		}
		else {
			return __builtin_nanl("");
		}
	}
	else {
		bool inf = false;
		if (str_case_cmp(ptr, "infinity", 8) == 0) {
			ptr += 8;
			inf = true;
		}
		else if (str_case_cmp(ptr, "inf", 3) == 0) {
			ptr += 3;
			inf = true;
		}

		if (inf) {
			if (end_ptr) {
				*end_ptr = const_cast<C*>(ptr);
			}

			if constexpr (sizeof(T) == 4) {
				return __builtin_inff();
			}
			else if constexpr (sizeof(T) == 8) {
				return __builtin_inf();
			}
			else {
				return __builtin_infl();
			}
		}
	}

	int base = 10;
	if (*ptr == '0' && tolower_helper(ptr[1]) == 'x') {
		ptr += 2;
		base = 16;
	}

	auto valid_char = [&](C c) {
		if (base == 10) {
			return c >= '0' && c <= '9';
		}
		else {
			return (c >= '0' && c <= '9') || ((c | 1 << 5) >= 'a' && (c | 1 << 5) <= 'f');
		}
	};

	T value {};
	while (valid_char(*ptr)) {
		auto c = *ptr++;
		auto digit = c <= '9' ? (c - '0') : (tolower_helper(c) - 'a' + 10);
		value *= base;
		value += digit;
	}

	if (*ptr == '.') {
		++ptr;

		T decimal {static_cast<T>(base)};
		while (valid_char(*ptr)) {
			auto c = *ptr++;
			auto digit = c <= '9' ? (c - '0') : (tolower_helper(c) - 'a' + 10);
			value += static_cast<T>(digit) / decimal;
			decimal *= base;
		}
	}

	char exponent_char;
	if (base == 10) {
		exponent_char = 'e';
	}
	else {
		exponent_char = 'p';
	}

	if (tolower_helper(*ptr) == exponent_char) {
		++ptr;

		bool exponent_sign = false;
		if (*ptr == '-') {
			++ptr;
			exponent_sign = true;
		}
		else if (*ptr == '+') {
			++ptr;
		}

		int exponent = 0;
		while (*ptr >= '0' && *ptr <= '9') {
			auto digit = *ptr++ - '0';
			exponent *= 10;
			exponent += digit;
		}

		int exponent_value = base == 10 ? 10 : 2;
		if (exponent_sign) {
			for (int i = 0; i < exponent; ++i) {
				value /= exponent_value;
			}
		}
		else {
			for (int i = 0; i < exponent; ++i) {
				value *= exponent_value;
			}
		}
	}

	if (isinf(value)) {
		errno = ERANGE;
	}

	if (end_ptr) {
		*end_ptr = const_cast<C*>(ptr);
	}

	return sign ? -value : value;
}
