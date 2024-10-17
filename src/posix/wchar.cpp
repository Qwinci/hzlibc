#include "wchar.h"
#include "utils.hpp"
#include "stdlib.h"
#include "wctype.h"
#include "internal/string.hpp"

#define memcpy __builtin_memcpy

EXPORT size_t wcsnlen(const wchar_t* str, size_t max_len) {
	size_t len = 0;
	for (; max_len && *str; ++str, --max_len) ++len;
	return len;
}

EXPORT int wcscasecmp(const wchar_t* s1, const wchar_t* s2) {
	for (;; ++s1, ++s2) {
		wint_t res = towlower(*s1) - towlower(*s2);
		if (res != 0 || !*s1) {
			return static_cast<int>(res);
		}
	}
}

EXPORT int wcwidth(wchar_t ch) {
	if (ch > 0x7F) {
		println("wcwidth: wide char support is not implemented");
	}
	return 1;
}

EXPORT int wcswidth(const wchar_t* str, size_t len) {
	int columns = 0;
	for (size_t i = 0; i < len && *str; ++i) {
		int width = wcwidth(*str++);
		if (width < 0) {
			return -1;
		}
		columns += width;
	}
	return columns;
}

EXPORT int wcscoll_l(const wchar_t* lhs, const wchar_t* rhs, locale_t locale) {
	println("wcscoll_l ignores locale");
	return wcscoll(lhs, rhs);
}

EXPORT wchar_t* wcsdup(const wchar_t* str) {
	size_t len = wcslen(str);
	auto* mem = static_cast<wchar_t*>(malloc((len + 1) * sizeof(wchar_t)));
	if (!mem) {
		return nullptr;
	}
	memcpy(mem, str, (len + 1) * sizeof(wchar_t));
	return mem;
}

EXPORT size_t wcsxfrm_l(
	wchar_t* __restrict dest,
	const wchar_t* __restrict src,
	size_t count,
	locale_t locale) {
	println("wcsxfrm_l ignores locale");
	return wcsxfrm(dest, src, count);
}

EXPORT size_t wcsftime_l(
	wchar_t* __restrict str,
	size_t count,
	const wchar_t* __restrict fmt,
	const struct tm* time,
	locale_t locale) {
	println("wcsftime_l ignores locale");
	return wcsftime(str, count, fmt, time);
}

EXPORT size_t wcsnrtombs(
	char* __restrict dest,
	const wchar_t** __restrict src,
	size_t num_wc,
	size_t len,
	mbstate_t* __restrict ps) {
	size_t written = 0;
	for (size_t i = 0; i < num_wc; ++i) {
		auto wc = **src;
		char buf[4];
		size_t bytes = wcrtomb(buf, wc, ps);
		if (bytes == static_cast<size_t>(-1)) {
			return static_cast<size_t>(-1);
		}
		if (dest) {
			if (written + bytes > len) {
				return written;
			}
			else {
				memcpy(dest, buf, bytes);
				dest += bytes;
			}
		}

		written += bytes;

		if (wc == 0) {
			*src = nullptr;
			break;
		}
		else {
			++*src;
		}
	}

	return written;
}

EXPORT size_t mbsnrtowcs(
	wchar_t* __restrict dest,
	const char** __restrict src,
	size_t num_chars,
	size_t len,
	mbstate_t* __restrict ps) {
	return internal::mbsnrtowcs(dest, src, num_chars, len, ps);
}

ALIAS(wcscoll_l, __wcscoll_l);
ALIAS(wcsxfrm_l, __wcsxfrm_l);
ALIAS(wcsftime_l, __wcsftime_l);
