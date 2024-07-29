#include "wchar.h"
#include "utils.hpp"

#define memcpy __builtin_memcpy

EXPORT int wcwidth(wchar_t ch) {
	if (ch > 0x7F) {
		println("wcwidth: wide char support is not implemented");
	}
	return 1;
}

EXPORT int wcscoll_l(const wchar_t* lhs, const wchar_t* rhs, locale_t locale) {
	println("wcscoll_l ignores locale");
	return wcscoll(lhs, rhs);
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
	size_t written = 0;
	for (size_t i = 0; i < num_chars;) {
		wchar_t buf;
		size_t bytes_used = mbrtowc(&buf, *src, num_chars - i, ps);
		if (bytes_used == static_cast<size_t>(-1)) {
			return static_cast<size_t>(-1);
		}
		else if (bytes_used == static_cast<size_t>(-2)) {
			break;
		}

		if (dest) {
			if (written + 1 > len) {
				return written;
			}
			else {
				*dest++ = buf;
			}
		}

		++written;

		if (buf == 0) {
			*src = nullptr;
			break;
		}
		else {
			*src += bytes_used;
			i += bytes_used;
		}
	}

	return written;
}

ALIAS(wcscoll_l, __wcscoll_l);
ALIAS(wcsxfrm_l, __wcsxfrm_l);
ALIAS(wcsftime_l, __wcsftime_l);
