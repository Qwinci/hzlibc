#include "wchar.h"
#include "utils.hpp"
#include "errno.h"
#include "uchar.h"
#include "string.h"
#include "ansi/stdio_internal.hpp"
#include "stdio.h"
#include "internal/time.hpp"

EXPORT int mbsinit(const mbstate_t*) {
	return 1;
}

EXPORT size_t wcrtomb(char* __restrict str, wchar_t wc, mbstate_t* __restrict) {
	char buf[4];
	if (!str) {
		str = buf;
	}

	if (wc <= 0x7F) {
		*str = static_cast<char>(wc);
		return 1;
	}
	else if (wc <= 0x7FF) {
		*str++ = static_cast<char>(0b11000000 | (wc >> 6));
		*str = static_cast<char>(0b10000000 | (wc & 0x3F));
		return 2;
	}
	else if (wc <= 0xFFFF) {
		*str++ = static_cast<char>(0b11100000 | (wc >> 12));
		*str++ = static_cast<char>(0b10000000 | (wc >> 6 & 0x3F));
		*str = static_cast<char>(0b10000000 | (wc & 0x3F));
		return 3;
	}
	else if (wc <= 0x10FFFF) {
		*str++ = static_cast<char>(0b11110000 | (wc >> 18));
		*str++ = static_cast<char>(0b10000000 | (wc >> 12 & 0x3F));
		*str++ = static_cast<char>(0b10000000 | (wc >> 6 & 0x3F));
		*str = static_cast<char>(0b10000000 | (wc & 0x3F));
		return 4;
	}
	else {
		errno = EILSEQ;
		return static_cast<size_t>(-1);
	}
}

EXPORT size_t mbrtowc(
	wchar_t* __restrict pwc,
	const char* __restrict str,
	size_t len,
	mbstate_t* __restrict ps) {
	auto* pc32 = reinterpret_cast<char32_t*>(pwc);
	return mbrtoc32(pc32, str, len, ps);
}

EXPORT size_t mbsrtowcs(
	wchar_t* __restrict dest,
	const char** __restrict src,
	size_t len,
	mbstate_t* __restrict ps) {
	return mbsnrtowcs(dest, src, SIZE_MAX, len, ps);
}

EXPORT int wctob(wint_t ch) {
	if (ch <= 0x7F) {
		return static_cast<int>(ch);
	}
	else {
		return -1;
	}
}

EXPORT wint_t btowc(int ch) {
	if (ch < 0) {
		return WEOF;
	}
	else {
		return static_cast<wint_t>(ch);
	}
}

EXPORT size_t wcslen(const wchar_t* str) {
	size_t len = 0;
	while (*str++) ++len;
	return len;
}

EXPORT int wcscmp(const wchar_t* lhs, const wchar_t* rhs) {
	for (;; ++lhs, ++rhs) {
		int res = *lhs - *rhs;
		if (res != 0 || !*lhs) {
			return res;
		}
	}
}

EXPORT wchar_t* wmemchr(const wchar_t* ptr, wchar_t ch, size_t count) {
	for (; count; ++ptr, --count) {
		if (*ptr == ch) {
			return const_cast<wchar_t*>(ptr);
		}
	}
	return nullptr;
}

EXPORT wchar_t* wmemcpy(wchar_t* __restrict dest, const wchar_t* __restrict src, size_t count) {
	return static_cast<wchar_t*>(memcpy(dest, src, count * sizeof(wchar_t)));
}

EXPORT wchar_t* wmemmove(wchar_t* dest, const wchar_t* src, size_t count) {
	return static_cast<wchar_t*>(memmove(dest, src, count * sizeof(wchar_t)));
}

EXPORT wchar_t* wmemset(wchar_t* dest, wchar_t ch, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		dest[i] = ch;
	}
	return dest;
}

EXPORT int wmemcmp(const wchar_t* lhs, const wchar_t* rhs, size_t count) {
	for (; count; --count, ++lhs, ++rhs) {
		int res = *lhs - *rhs;
		if (res != 0) {
			return res;
		}
	}
	return 0;
}

EXPORT int wcscoll(const wchar_t* lhs, const wchar_t* rhs) {
	println("wcscoll ignores locale");
	return wcscmp(lhs, rhs);
}

EXPORT size_t wcsxfrm(wchar_t* __restrict dest, const wchar_t* __restrict src, size_t count) {
	println("wcsxfrm ignores locale");
	size_t len = wcslen(src);
	if (dest && count) {
		if (len + 1 < count) {
			memcpy(dest, src, (len + 1) * sizeof(wchar_t));
		}
		else {
			memcpy(dest, src, (count - 1) * sizeof(wchar_t));
			dest[count - 1] = 0;
		}
	}
	return len;
}

static constexpr wchar_t CHARS[] = L"0123456789ABCDEF";
static constexpr hz::wstring_view MONTH_ABBREVS[] {
	L"Jan",
	L"Feb",
	L"Mar",
	L"Apr",
	L"May",
	L"Jun",
	L"Jul",
	L"Aug",
	L"Sep",
	L"Oct",
	L"Nov",
	L"Dec"
};

static constexpr hz::wstring_view MONTHS[] {
	L"January",
	L"February",
	L"March",
	L"April",
	L"May",
	L"June",
	L"July",
	L"August",
	L"September",
	L"October",
	L"November",
	L"December"
};

static constexpr hz::wstring_view DAY_ABBREVS[] {
	L"Sun",
	L"Mon",
	L"Tue",
	L"Wed",
	L"Thu",
	L"Fri",
	L"Sat"
};

static constexpr hz::wstring_view DAYS[] {
	L"Sunday",
	L"Monday",
	L"Tuesday",
	L"Wednesday",
	L"Thursday",
	L"Friday",
	L"Saturday"
};

static constexpr hz::wstring_view FIN_MONTH_ABBREVS[] {
	L"tammi",
	L"helmi",
	L"maalis",
	L"huhti",
	L"touko",
	L"kesä",
	L"heinä",
	L"elo",
	L"syys",
	L"loka",
	L"marras",
	L"joulu"
};

static constexpr hz::wstring_view FIN_MONTHS[] {
	L"tammikuu",
	L"helmikuu",
	L"maaliskuu",
	L"huhtikuu",
	L"toukokuu",
	L"kesäkuu",
	L"heinäkuu",
	L"elokuu",
	L"syyskuu",
	L"lokakuu",
	L"marraskuu",
	L"joulukuu"
};

EXPORT size_t wcsftime(
	wchar_t* __restrict str,
	size_t count,
	const wchar_t* __restrict fmt,
	const struct tm* time) {
	// todo this should properly take locale into account

	size_t written = 0;

	auto write = [&](const wchar_t* ptr, size_t len) {
		if (written + len > count) {
			return false;
		}
		memcpy(str, ptr, len * sizeof(wchar_t));
		str += len;
		written += len;
		return true;
	};

	auto write_int = [&](int value, int base, int pad) {
		wchar_t buf[64];
		wchar_t* ptr = buf + 64;
		do {
			*--ptr = CHARS[value % base];
			value /= base;
		} while (value);
		int len = static_cast<int>((buf + 64) - ptr);
		if (len < pad) {
			pad -= len;
			for (; pad; --pad) {
				*--ptr = L'0';
			}
		}

		return write(ptr, (buf + 64) - ptr);
	};

	while (true) {
		auto* start = fmt;
		for (; *fmt && *fmt != '%'; ++fmt);
		size_t len = fmt - start;
		for (; *fmt == '%' && fmt[1] == '%'; fmt += 2) ++len;
		if (len && !write(start, len)) {
			return 0;
		}

		if (!*fmt) {
			break;
		}

		++fmt;
		switch (*fmt) {
			case L'n':
				++fmt;
				if (!write(L"\n", 1)) {
					return 0;
				}
				break;
			case L't':
				++fmt;
				if (!write(L"\t", 1)) {
					return 0;
				}
				break;
			case L'Y':
				++fmt;
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			case L'E':
				++fmt;
				if (*fmt == L'Y') {
					++fmt;
					if (!write_int(time->tm_year + 1900, 10, 0)) {
						return 0;
					}
				}
				else if (*fmt == L'y') {
					++fmt;
					// todo this should be the offset from nl_langinfo with ERA
					int year = time->tm_year + 1900;
					int era = year / 1000 * 1000;
					year -= era;
					if (!write_int(year, 10, 2)) {
						return 0;
					}
				}
				else if (*fmt == L'C') {
					++fmt;
				}
				else if (*fmt == L'c') {
					++fmt;
				}
				else if (*fmt == L'X') {
					++fmt;
				}
				else {
					return 0;
				}
				break;
			case L'y':
			{
				++fmt;
				int year = time->tm_year + 1900;
				wchar_t chars[] {CHARS[year / 10 % 10], CHARS[year % 10]};
				if (!write(chars, 2)) {
					return 0;
				}
				break;
			}
			case L'O':
				++fmt;
				if (*fmt == L'y') {
					++fmt;
				}
				else if (*fmt == L'b') {
					++fmt;
				}
				else if (*fmt == L'B') {
					++fmt;
				}
				else if (*fmt == L'm') {
					++fmt;
				}
				else if (*fmt == L'd') {
					++fmt;
				}
				else if (*fmt == L'e') {
					++fmt;
				}
				else if (*fmt == L'w') {
					++fmt;
				}
				else if (*fmt == L'u') {
					++fmt;
				}
				else if (*fmt == L'H') {
					++fmt;
				}
				else if (*fmt == L'I') {
					++fmt;
				}
				else if (*fmt == L'M') {
					++fmt;
				}
				else if (*fmt == L'S') {
					++fmt;
				}
				else {
					return 0;
				}
				break;
			case L'C':
			{
				++fmt;
				int year = time->tm_year + 1900;
				wchar_t chars[] {CHARS[year / 10 / 10 / 10 % 10], CHARS[year / 10 / 10 % 10]};
				if (!write(chars, 2)) {
					return 0;
				}
				break;
			}
			case L'G':
			case L'g':
			{
				++fmt;
				// todo
				break;
			}
			case L'b':
			case L'h':
				++fmt;
				if (!write(MONTH_ABBREVS[time->tm_mon].data(), MONTH_ABBREVS[time->tm_mon].size())) {
					return 0;
				}
				break;
			case L'B':
				++fmt;
				if (!write(MONTHS[time->tm_mon].data(), MONTHS[time->tm_mon].size())) {
					return 0;
				}
				break;
			case L'm':
				++fmt;
				if (!write_int(time->tm_mon + 1, 10, 2)) {
					return 0;
				}
				break;
			case L'j':
				++fmt;
				if (!write_int(time->tm_yday + 1, 10, 3)) {
					return 0;
				}
				break;
			case L'd':
				++fmt;
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				break;
			case L'e':
			{
				++fmt;
				wchar_t chars[] {CHARS[time->tm_mday / 10 % 10], CHARS[time->tm_mday % 10]};
				if (chars[0] == '0') {
					if (!write(&chars[1], 1)) {
						return 0;
					}
				}
				else {
					if (!write(chars, 2)) {
						return 0;
					}
				}
				break;
			}
			case L'a':
				++fmt;
				if (!write(DAY_ABBREVS[time->tm_wday].data(), DAY_ABBREVS[time->tm_wday].size())) {
					return 0;
				}
				break;
			case L'A':
				++fmt;
				if (!write(DAYS[time->tm_wday].data(), DAYS[time->tm_wday].size())) {
					return 0;
				}
				break;
			case L'w':
				++fmt;
				if (!write_int(time->tm_wday, 10, 0)) {
					return 0;
				}
				break;
			case L'u':
				++fmt;
				if (!write_int(time->tm_wday + 1, 10, 0)) {
					return 0;
				}
				break;
			case L'H':
				++fmt;
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				break;
			case L'I':
			{
				++fmt;
				int hour = time->tm_hour % 12;
				if (hour == 0) {
					hour = 12;
				}
				if (!write_int(hour, 10, 0)) {
					return 0;
				}
				break;
			}
			case L'M':
				++fmt;
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				break;
			case L'S':
				++fmt;
				if (!write_int(time->tm_sec, 10, 2)) {
					return 0;
				}
				break;
			case L'c':
			{
				++fmt;
				if (!write(DAY_ABBREVS[time->tm_wday].data(), DAY_ABBREVS[time->tm_wday].size())) {
					return 0;
				}
				if (!write(L" ", 1)) {
					return 0;
				}
				if (!write(MONTH_ABBREVS[time->tm_mon].data(), MONTH_ABBREVS[time->tm_mon].size())) {
					return 0;
				}
				if (!write(L" ", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mday, 10, 0)) {
					return 0;
				}
				if (!write(L" ", 1)) {
					return 0;
				}
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				if (!write(L":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				if (!write(L":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_sec, 10, 2)) {
					return 0;
				}
				if (!write(L" ", 1)) {
					return 0;
				}
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			}
			case L'x':
				++fmt;
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				if (!write(L".", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mon + 1, 10, 0)) {
					return 0;
				}
				if (!write(L".", 1)) {
					return 0;
				}
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			case L'D':
				++fmt;
				if (!write_int(time->tm_mon + 1, 10, 0)) {
					return 0;
				}
				if (!write(L"/", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				if (!write(L"/", 1)) {
					return 0;
				}
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				break;
			case L'F':
				++fmt;
				if (!write_int(time->tm_year + 1900, 10, 0)) {
					return 0;
				}
				if (!write(L"-", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mon + 1, 10, 0)) {
					return 0;
				}
				if (!write(L"-", 1)) {
					return 0;
				}
				if (!write_int(time->tm_mday, 10, 2)) {
					return 0;
				}
				break;
			case L'r':
			{
				++fmt;
				int hour = time->tm_hour % 12;
				if (hour == 0) {
					hour = 12;
				}
				if (!write_int(hour, 10, 0)) {
					return 0;
				}
				if (!write(L":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				break;
			}
			case L'R':
			{
				++fmt;
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				if (!write(L":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				break;
			}
			case L'T':
			{
				++fmt;
				if (!write_int(time->tm_hour, 10, 2)) {
					return 0;
				}
				if (!write(L":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_min, 10, 2)) {
					return 0;
				}
				if (!write(L":", 1)) {
					return 0;
				}
				if (!write_int(time->tm_sec, 10, 2)) {
					return 0;
				}
				break;
			}
			case L'p':
				if (time->tm_hour < 12) {
					if (!write(L"AM", 2)) {
						return 0;
					}
				}
				else {
					if (!write(L"PM", 2)) {
						return 0;
					}
				}
				break;
			case L'z':
			{
				if (tz_data_initialized()) {
					tm copy = *time;
					auto t = mktime(&copy);
					if (t != -1) {
						auto info = get_tz_data(t);
						if (info.gmt_offset < 0) {
							info.gmt_offset *= -1;
							if (!write(L"-", 1)) {
								return 0;
							}
						}
						else {
							if (!write(L"+", 1)) {
								return 0;
							}
						}
						int hour = info.gmt_offset / (60 * 60);
						int min = info.gmt_offset % (60 * 60) / 60;

						if (!write_int(hour, 10, 2)) {
							return 0;
						}
						if (!write_int(min, 10, 2)) {
							return 0;
						}
					}
				}
				break;
			}
			case 'Z':
			{
				if (tz_data_initialized()) {
					tm copy = *time;
					auto t = mktime(&copy);
					if (t != -1) {
						auto info = get_tz_data(t);
						auto zone_len = strlen(info.zone);
						for (size_t i = 0; i < zone_len; ++i) {
							wchar_t w = info.zone[i];
							if (!write(&w, 1)) {
								return 0;
							}
						}
					}
				}
				break;
			}
			default:
				if (!write(fmt - 1, 2)) {
					return 0;
				}
				++fmt;
				break;
		}
	}

	if (count) {
		if (written < count) {
			*str = 0;
		}
		else {
			return 0;
		}
	}

	return written;
}

EXPORT wint_t fgetwc(FILE* file) {
	auto guard = file->mutex.lock();

	char buf[4];
	char* ptr = buf;
	while (ptr < buf + 4) {
		auto c = fgetc_unlocked(file);
		if (c < 0) {
			if (ptr != buf) {
				file->flags |= FILE_ERR_FLAG;
				errno = EILSEQ;
			}
			return WEOF;
		}
		*ptr = static_cast<char>(c);
		wchar_t wc;
		size_t bytes = mbrtowc(&wc, buf, ptr - buf, nullptr);
		if (bytes == static_cast<size_t>(-1)) {
			if (ptr != buf) {
				file->flags |= FILE_ERR_FLAG;
				errno = EILSEQ;
			}
			return WEOF;
		}
		else if (bytes > 0) {
			return wc;
		}
		++ptr;
	}

	errno = EILSEQ;
	return WEOF;
}

EXPORT wint_t getwc(FILE* file) {
	return fgetwc(file);
}

EXPORT wint_t fputwc(wint_t ch, FILE* file) {
	char buf[4];
	size_t bytes = wcrtomb(buf, static_cast<wchar_t>(ch), nullptr);
	if (bytes == static_cast<size_t>(-1)) {
		return WEOF;
	}
	if (fwrite(buf, bytes, 1, file) != bytes) {
		return WEOF;
	}
	return ch;
}

EXPORT wint_t putwc(wint_t ch, FILE* file) {
	return fputwc(ch, file);
}

EXPORT wint_t ungetwc(wint_t ch, FILE* file) {
	char buf[4];
	size_t bytes = wcrtomb(buf, static_cast<wchar_t>(ch), nullptr);
	if (bytes == static_cast<size_t>(-1)) {
		return WEOF;
	}
	for (size_t i = 0; i < bytes; ++i) {
		if (ungetc(buf[i], file) == EOF) {
			for (size_t j = 0; j < i; ++j) {
				fgetc_unlocked(file);
			}
			return WEOF;
		}
	}
	return ch;
}

EXPORT int vswprintf(
	wchar_t* __restrict buffer,
	size_t size,
	const wchar_t* __restrict fmt,
	va_list ap) {
	panic("vswprintf is not implemented");
}

EXPORT int swprintf(
	wchar_t* __restrict buffer,
	size_t size,
	const wchar_t* __restrict fmt,
	...) {
	va_list ap;
	va_start(ap, fmt);
	int res = vswprintf(buffer, size, fmt, ap);
	va_end(ap);
	return res;
}
