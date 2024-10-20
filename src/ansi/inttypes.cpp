#include "inttypes.h"
#include "utils.hpp"
#include "ctype.h"
#include "errno.h"

namespace {
	constexpr char CHARS[] = "0123456789abcdefghijklmnopqrstuvwxyz";
}

EXPORT uintmax_t strtoumax(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	auto start = ptr;

	while (isspace(*ptr)) {
		++ptr;
	}

	auto after_space = ptr;

	bool sign = false;
	if (*ptr == '-') {
		++ptr;
		sign = true;
	}
	else if (*ptr == '+') {
		++ptr;
	}

	if (base == 0) {
		if (ptr[0] == '0' && tolower(ptr[1]) == 'x') {
			base = 16;
			ptr += 2;
		}
		else if (ptr[0] == '0' && tolower(ptr[1]) == 'b') {
			base = 2;
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
		if (ptr[0] == '0' && tolower(ptr[1]) == 'b') {
			ptr += 2;
		}
	}
	else if (base == 8) {
		if (*ptr == '0') {
			++ptr;
		}
	}
	else if (base == 16) {
		if (ptr[0] == '0' && tolower(ptr[1]) == 'x') {
			ptr += 2;
		}
	}

	uintmax_t value = 0;
	if (base <= 36) {
		for (; *ptr >= '0' && tolower(*ptr) <= CHARS[base - 1]; ++ptr) {
			auto old = value;
			value *= base;
			if (value / base != old) {
				errno = ERANGE;
				return UINTMAX_MAX;
			}
			old = value;
			value += *ptr <= '9' ? (*ptr - '0') : (tolower(*ptr) - 'a' + 10);
			if (value < old) {
				errno = ERANGE;
				return UINTMAX_MAX;
			}
		}
	}

	if (end_ptr) {
		if (after_space != ptr) {
			*end_ptr = const_cast<char*>(ptr);
		}
		else {
			*end_ptr = const_cast<char*>(start);
		}
	}
	if (sign) {
		value = -value;
	}
	return value;
}

EXPORT intmax_t strtoimax(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	auto start = ptr;

	while (isspace(*ptr)) {
		++ptr;
	}

	auto after_space = ptr;

	bool sign = false;
	if (*ptr == '-') {
		++ptr;
		sign = true;
	}
	else if (*ptr == '+') {
		++ptr;
	}

	if (base == 0) {
		if (ptr[0] == '0' && tolower(ptr[1]) == 'x') {
			base = 16;
			ptr += 2;
		}
		else if (ptr[0] == '0' && tolower(ptr[1]) == 'b') {
			base = 2;
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
		if (ptr[0] == '0' && tolower(ptr[1]) == 'b') {
			ptr += 2;
		}
	}
	else if (base == 8) {
		if (*ptr == '0') {
			++ptr;
		}
	}
	else if (base == 16) {
		if (ptr[0] == '0' && tolower(ptr[1]) == 'x') {
			ptr += 2;
		}
	}

	uintmax_t value = 0;
	if (base <= 36) {
		for (; *ptr >= '0' && tolower(*ptr) <= CHARS[base - 1]; ++ptr) {
			auto old = value;
			value *= base;
			if (value / base != old) {
				errno = ERANGE;
				return INTMAX_MAX;
			}
			old = value;
			value += *ptr <= '9' ? (*ptr - '0') : (tolower(*ptr) - 'a' + 10);
			if (value < old) {
				errno = ERANGE;
				return INTMAX_MAX;
			}
		}
	}

	if (value > INTMAX_MAX) {
		errno = ERANGE;
		return INTMAX_MAX;
	}

	if (end_ptr) {
		if (after_space != ptr) {
			*end_ptr = const_cast<char*>(ptr);
		}
		else {
			*end_ptr = const_cast<char*>(start);
		}
	}
	if (sign) {
		value = -value;
	}
	return static_cast<intmax_t>(value);
}

EXPORT intmax_t imaxabs(intmax_t x) {
	return x < 0 ? x * -1 : x;
}

EXPORT imaxdiv_t imaxdiv(intmax_t x, intmax_t y) {
	return {
		.quot = x / y,
		.rem = x % y
	};
}

ALIAS(strtoumax, __isoc23_strtoumax);
ALIAS(strtoimax, __isoc23_strtoimax);
