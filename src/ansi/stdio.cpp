#include "stdio.h"
#include "utils.hpp"
#include "sys.hpp"
#include "string.h"
#include "stdio_internal.hpp"
#include "stdio_unlocked.hpp"
#include "fcntl.h"
#include "errno.h"
#include "ctype.h"
#include "math.h"
#include "wchar.h"
#include <hz/new.hpp>
#include <hz/algorithm.hpp>
#include <hz/string_utils.hpp>
#include <limits.h>
#include <float.h>

ssize_t fd_file_write(FILE* file, const void* data, size_t size) {
	ssize_t written;
	if (auto err = sys_write(file->fd, data, size, &written)) {
		file->error = err;
		file->flags |= FILE_ERR_FLAG;
		return -1;
	}
	file->last_was_read = false;
	return written;
}

ssize_t fd_file_read(FILE* file, void* data, size_t size) {
	ssize_t unget = 0;
	auto* ptr = static_cast<unsigned char*>(data);
	while (size && file->ungetc_size) {
		auto c = *--file->ungetc_ptr;
		*ptr++ = c;
		--file->ungetc_size;
		--size;
		++unget;
	}
	data = ptr;

	ssize_t count_read;
	if (auto err = sys_read(file->fd, data, size, &count_read)) {
		file->error = err;
		file->flags |= FILE_ERR_FLAG;
		return -1;
	}
	else if (count_read < static_cast<ssize_t>(size)) {
		file->flags |= FILE_EOF_FLAG;
	}
	file->last_was_read = true;
	return count_read + unget;
}

namespace {
	FILE STDIN {
		.write = fd_file_write,
		.read = fd_file_read,
		.fd = 0,
		.last_was_read = true,
		.no_destroy = true
	};

	FILE STDOUT {
		.write = fd_file_write,
		.read = fd_file_read,
		.fd = 1,
		.no_destroy = true
	};

	FILE STDERR {
		.write = fd_file_write,
		.read = fd_file_read,
		.fd = 2,
		.no_destroy = true
	};
}

FILE* create_fd_file(int fd) {
	return new FILE {
		.write = fd_file_write,
		.read = fd_file_read,
		.fd = fd
	};
}

EXPORT FILE* stdin = &STDIN;
EXPORT FILE* stdout = &STDOUT;
EXPORT FILE* stderr = &STDERR;

EXPORT int fputc(int ch, FILE* file) {
	auto c = static_cast<unsigned char>(ch);

	auto guard = file->mutex.lock();
	if (file->write(file, &c, sizeof(c)) < 0) {
		return EOF;
	}
	return ch;
}

ALIAS(fputc, putc);

EXPORT int putchar(int ch) {
	return fputc(ch, stdout);
}

EXPORT int fputs(const char* __restrict str, FILE* __restrict file) {
	auto guard = file->mutex.lock();
	if (file->write(file, str, strlen(str)) < 0) {
		return EOF;
	}
	return 0;
}

EXPORT int puts(const char* str) {
	if (auto err = fputs(str, stdout); err < 0) {
		return err;
	}
	if (auto err = fputc('\n', stdout); err < 0) {
		return err;
	}
	return 0;
}

EXPORT void perror(const char* str) {
	if (str) {
		fprintf(stderr, "%s: %s\n", str, strerror(errno));
	}
	else {
		fprintf(stderr, "%s\n", strerror(errno));
	}
}

enum class State {
	None,
	hh,
	h,
	l,
	ll,
	j,
	z,
	t,
	L
};

namespace flags {
	static constexpr int LJUST = 1 << 0;
	static constexpr int SIGN = 1 << 1;
	static constexpr int SPACE = 1 << 2;
	static constexpr int ALT = 1 << 3;
	static constexpr int ZERO = 1 << 4;
}

static constexpr char CHARS[] = "0123456789ABCDEF";
static constexpr char LOWER_CHARS[] = "0123456789abcdef";

EXPORT int vfprintf(FILE* __restrict file, const char* __restrict fmt, va_list ap) {
	size_t written = 0;

	auto write = [&](const char* ptr, size_t len) {
		if (file) {
			auto tmp = file->write(file, ptr, len);
			if (tmp < 0) {
				file->mutex.manual_unlock();
				return false;
			}
			written += static_cast<size_t>(tmp);
		}
		else {
			written += len;
		}
		if (written > INT_MAX) {
			if (file) {
				file->mutex.manual_unlock();
			}
			errno = EOVERFLOW;
			return false;
		}
		return true;
	};

	struct IntOpts {
		int min_width;
		int precision;
		bool negative;
		int always_write_sign;
		int plus_as_space;
		int pad_with_zero;
		int right_pad;
		bool upper;
		int write_prefix;
	};

	auto pad = [&](int min_width, int written, char pad_c) {
		if (written < min_width) {
			for (int i = 0; i < min_width - written; ++i) {
				if (!write(&pad_c, 1)) {
					return false;
				}
			}
		}

		return true;
	};

	auto write_int = [&](
		uintmax_t value,
		int base,
		IntOpts opts) {
		char buf[64];
		char* ptr = buf + 64;

		int len;
		if (value == 0 && opts.precision == 0) {
			len = 0;
		}
		else {
			if (opts.upper) {
				do {
					*--ptr = CHARS[value % base];
					value /= base;
				} while (value);
			}
			else {
				do {
					*--ptr = LOWER_CHARS[value % base];
					value /= base;
				} while (value);
			}

			len = static_cast<int>((buf + 64) - ptr);
		}

		int actual_width = hz::max(len, opts.precision);
		if (opts.negative || opts.always_write_sign) {
			++actual_width;
		}

		if (opts.write_prefix && len) {
			if (base == 2 || base == 16) {
				actual_width += 2;
			}
			else if (base == 8) {
				actual_width += 1;
			}
		}

		char pad_c = opts.pad_with_zero ? '0' : ' ';

		if (!opts.right_pad && actual_width < opts.min_width) {
			for (int i = 0; i < opts.min_width - actual_width; ++i) {
				if (!write(&pad_c, 1)) {
					return false;
				}
			}
		}

		if (opts.negative) {
			if (!write("-", 1)) {
				return false;
			}
		}
		else if (opts.always_write_sign) {
			if (!write("+", 1)) {
				return false;
			}
		}
		else if (opts.plus_as_space) {
			if (!write(" ", 1)) {
				return false;
			}
		}

		if (opts.write_prefix && len) {
			if (base == 2) {
				if (!write(opts.upper ? "0B" : "0b", 2)) {
					return false;
				}
			}
			else if (base == 8) {
				if (!write("0", 1)) {
					return false;
				}
			}
			else if (base == 16) {
				if (!write(opts.upper ? "0X" : "0x", 2)) {
					return false;
				}
			}
		}

		if (len < opts.precision) {
			for (int i = 0; i < opts.precision - len; ++i) {
				if (!write("0", 1)) {
					return false;
				}
			}
		}

		if (!write(ptr, len)) {
			return false;
		}

		if (opts.right_pad && actual_width < opts.min_width) {
			for (int i = 0; i < opts.min_width - actual_width; ++i) {
				if (!write(&pad_c, 1)) {
					return false;
				}
			}
		}

		return true;
	};

	if (file) {
		file->mutex.manual_lock();
	}

	while (true) {
		auto* start = fmt;
		for (; *fmt && *fmt != '%'; ++fmt);
		size_t len = fmt - start;
		for (; *fmt == '%' && fmt[1] == '%'; fmt += 2) ++len;
		if (len) {
			if (!write(start, len)) {
				return -1;
			}

			continue;
		}

		if (!*fmt) {
			if (file) {
				file->mutex.manual_unlock();
			}
			return static_cast<int>(written);
		}

		++fmt;

		int flags = 0;
		while (true) {
			auto c = *fmt;
			if (c == '-') {
				++fmt;
				flags |= flags::LJUST;
			}
			else if (c == '+') {
				++fmt;
				flags |= flags::SIGN;
			}
			else if (c == ' ') {
				++fmt;
				if (!(flags & flags::SIGN)) {
					flags |= flags::SPACE;
				}
			}
			else if (c == '#') {
				++fmt;
				flags |= flags::ALT;
			}
			else if (c == '0') {
				++fmt;
				if (!(flags & flags::LJUST)) {
					flags |= flags::ZERO;
				}
			}
			else {
				break;
			}
		}

		int width = 0;
		bool has_width = false;
		if (isdigit(*fmt)) {
			size_t count;
			width = hz::to_integer<int>(hz::string_view {fmt}, 10, &count);
			fmt += count;
			has_width = true;
		}
		else if (*fmt == '*') {
			++fmt;
			width = va_arg(ap, int);
			has_width = true;

			if (width < 0) {
				width *= -1;
				flags |= flags::LJUST;
			}
		}

		int precision = 0;
		bool has_precision = false;
		if (*fmt == '.') {
			++fmt;
			if (isdigit(*fmt)) {
				size_t count;
				precision = hz::to_integer<int>(hz::string_view {fmt}, 10, &count);
				fmt += count;
				has_precision = true;
				flags &= ~flags::ZERO;
			}
			else if (*fmt == '*') {
				++fmt;
				precision = va_arg(ap, int);

				if (precision >= 0) {
					has_precision = true;
					flags &= ~flags::ZERO;
				}
			}
			else {
				has_precision = true;
				flags &= ~flags::ZERO;
			}
		}

		State state = State::None;
		bool processed = false;
		while (!processed) {
			auto c = *fmt;
			switch (c) {
				case 'h':
				{
					if (state == State::None) {
						++fmt;
						state = State::h;
					}
					else if (state == State::h) {
						++fmt;
						state = State::hh;
						processed = true;
					}
					else {
						processed = true;
					}
					break;
				}
				case 'l':
				{
					if (state == State::None) {
						++fmt;
						state = State::l;
					}
					else if (state == State::l) {
						++fmt;
						state = State::ll;
						processed = true;
					}
					else {
						processed = true;
					}
					break;
				}
				case 'j':
					if (state == State::None) {
						++fmt;
						state = State::j;
					}
					processed = true;
					break;
				case 'z':
					if (state == State::None) {
						++fmt;
						state = State::z;
					}
					processed = true;
					break;
				case 't':
					if (state == State::None) {
						++fmt;
						state = State::t;
					}
					processed = true;
					break;
				case 'L':
					if (state == State::None) {
						++fmt;
						state = State::L;
					}
					processed = true;
					break;
				default:
					processed = true;
					break;
			}
		}

		if (!*fmt) {
			if (file) {
				file->mutex.manual_unlock();
			}
			errno = EINVAL;
			return -1;
		}

		auto pop_signed_int = [&]() {
			intmax_t value;
			if (state == State::None ||
			    state == State::hh ||
			    state == State::h) {
				value = va_arg(ap, int);
			}
			else if (state == State::l) {
				value = va_arg(ap, long);
			}
			else if (state == State::ll) {
				value = va_arg(ap, long long);
			}
			else if (state == State::j) {
				value = va_arg(ap, intmax_t);
			}
			else if (state == State::z) {
				value = va_arg(ap, ssize_t);
			}
			else if (state == State::t) {
				value = va_arg(ap, ptrdiff_t);
			}
			else {
				panic("invalid printf %d state ", static_cast<int>(state));
			}

			return value;
		};

		auto pop_unsigned_int = [&]() {
			uintmax_t value;
			if (state == State::None ||
			    state == State::hh ||
			    state == State::h) {
				value = va_arg(ap, unsigned int);
			}
			else if (state == State::l) {
				value = va_arg(ap, unsigned long);
			}
			else if (state == State::ll) {
				value = va_arg(ap, unsigned long long);
			}
			else if (state == State::j) {
				value = va_arg(ap, uintmax_t);
			}
			else if (state == State::z) {
				value = va_arg(ap, size_t);
			}
			else if (state == State::t) {
				value = va_arg(ap, unsigned long);
			}
			else {
				panic("invalid printf %u state ", static_cast<int>(state));
			}

			return value;
		};

		switch (*fmt) {
			case 'd':
			case 'i':
			{
				++fmt;
				auto value = pop_signed_int();

				bool negative = false;
				if (value < 0) {
					value *= -1;
					negative = true;
				}

				IntOpts opts {
					.min_width = has_width ? width : 0,
					.precision = has_precision ? precision : 1,
					.negative = negative,
					.always_write_sign = flags & flags::SIGN,
					.plus_as_space = flags & flags::SPACE,
					.pad_with_zero = flags & flags::ZERO,
					.right_pad = flags & flags::LJUST,
					.upper = false,
					.write_prefix = false
				};

				if (!write_int(value, 10, opts)) {
					return -1;
				}

				break;
			}
			case 'f':
			case 'F':
			case 'e':
			case 'E':
			case 'g':
			case 'G':
			case 'a':
			case 'A':
			{
				auto specific = *fmt;
				auto specific_lower = specific | 1 << 5;
				bool upper = !(specific & 1 << 5);

				++fmt;

				long double value;
				if (state == State::L) {
					value = va_arg(ap, long double);
				}
				else {
					__ensure(state == State::None || state == State::l);
					value = va_arg(ap, double);
				}

				if (!isfinite(value)) {
					const char* str;
					if (isinf(value)) {
						str = upper ? "INF" : "inf";
					}
					else {
						str = upper ? "NAN" : "nan";
					}

					if (flags & flags::LJUST) {
						if (!write(str, 3)) {
							return -1;
						}
						if (!pad(width, 3, ' ')) {
							return -1;
						}
					}
					else {
						if (!pad(width, 3, ' ')) {
							return -1;
						}
						if (!write(str, 3)) {
							return -1;
						}
					}

					break;
				}

				bool negative = false;
				if (value < 0) {
					value = -value;
					negative = true;
				}

				int exponent;
				value = frexpl(value, &exponent) * 2;
				if (value != 0) {
					--exponent;
				}

				char buf[9 + LDBL_MANT_DIG / 4];
				char e_buf0[3 * sizeof(int)];
				char* e_buf = &e_buf0[3 * sizeof(int)];
				char* e_str;

				auto write_int_to_buf = [](uintmax_t value, char* ptr) {
					while (value) {
						*--ptr = static_cast<char>('0' + value % 10);
						value /= 10;
					}
					return ptr;
				};

				if (specific_lower == 'a') {
					if (has_precision && precision >= 0 && precision < (LDBL_MANT_DIG - 1 + 3) / 4) {
						double round = scalbn(1, LDBL_MANT_DIG - 1 - (precision * 4));
						if (negative) {
							value = -value;
							value -= round;
							value += round;
							value = -value;
						}
						else {
							value += round;
							value -= round;
						}
					}

					e_str = write_int_to_buf(exponent < 0 ? -exponent : exponent, e_buf);
					if (e_str == e_buf) {
						*--e_str = '0';
					}
					*--e_str = exponent < 0 ? '-' : '+';
					*--e_str = upper ? 'P' : 'p';

					static constexpr char HEX_CHARS[] = "0123456789ABCDEF";

					char* str = buf;
					do {
						int x = static_cast<int>(value);
						*str++ = static_cast<char>(HEX_CHARS[x] | (specific & 1 << 5));
						value = 16 * (value - x);
						if (str - buf == 1 &&
							(static_cast<bool>(value) ||
							precision > 0 ||
							(flags & flags::ALT))) {
							*str++ = '.';
						}
					} while (static_cast<bool>(value));

					int tmp_len;
					if (has_precision && precision && str - buf - 2 < precision) {
						tmp_len = static_cast<int>((precision + 2) + (e_buf - e_str));
					}
					else {
						tmp_len = static_cast<int>((str - buf) + (e_buf - e_str));
					}

					if (negative) {
						if (!write("-", 1)) {
							return -1;
						}
					}

					if (!write(upper ? "0X" : "0x", 2)) {
						return -1;
					}

					if (!write(buf, str - buf)) {
						return -1;
					}

					if (!pad(
						static_cast<int>(tmp_len - (e_buf - e_str) - (str - buf)),
						0,
						'0')) {
						return -1;
					}

					if (!write(e_str, e_buf - e_str)) {
						return -1;
					}

					break;
				}

				if (value != 0) {
					value *= 0x1p28;
					exponent -= 28;
				}

				uint32_t* a;
				uint32_t* d;
				uint32_t* r;
				uint32_t* z;
				uint32_t big[
					(LDBL_MANT_DIG + 28) / 29 + 1 +
					(LDBL_MAX_EXP + LDBL_MANT_DIG + 28 + 8) / 9];

				if (exponent < 0) {
					a = big;
					r = big;
					z = big;
				}
				else {
					a = big + sizeof(big) / sizeof(*big) - LDBL_MANT_DIG - 1;
					r = big + sizeof(big) / sizeof(*big) - LDBL_MANT_DIG - 1;
					z = big + sizeof(big) / sizeof(*big) - LDBL_MANT_DIG - 1;
				}

				do {
					*z = static_cast<uint32_t>(value);
					value = 1000000000 * (value - *z++);
				} while (static_cast<bool>(value));

				if (exponent > 0) {
					uint32_t carry = 0;
					int shift = hz::min(exponent, 29);
					for (d = z - 1; d >= a; --d) {
						uint64_t x = (static_cast<uint64_t>(*d) << shift) + carry;
						*d = x % 1000000000;
						carry = x / 1000000000;
					}
					if (carry) {
						*--a = carry;
					}
					while (z > a && !z[-1]) {
						--z;
					}
					exponent -= shift;
				}

				if (!has_precision) {
					precision = 6;
				}

				while (exponent < 0) {
					uint32_t carry = 0;
					uint32_t* b;
					int shift = hz::min(-exponent, 9);
					int need = 1 + (precision + LDBL_MANT_DIG / 3 + 8) / 9;
					for (d = a; d < z; ++d) {
						uint32_t rm = *d & ((1 << shift) - 1);
						*d = (*d >> shift) + carry;
						carry = (1000000000 >> shift) * rm;
					}
					if (!*a) {
						++a;
					}
					if (carry) {
						*z++ = carry;
					}

					b = specific_lower == 'f' ? r : a;
					if (z - b > need) {
						z = b + need;
					}
					exponent += shift;
				}

				if (a < z) {
					exponent = static_cast<int>(9 * (r - a));
					for (uint32_t i = 10; *a >= i; i *= 10, ++exponent);
				}
				else {
					exponent = 0;
				}

				// j == precision after radix (possibly negative)
				int j = precision - (specific_lower != 'f') * exponent - (specific_lower == 'g' && precision);
				if (j < 9 * (z - r - 1)) {
					d = r + 1 + ((j + 9 * LDBL_MAX_EXP) / 9 - LDBL_MAX_EXP);
					j += 9 * LDBL_MAX_EXP;
					j %= 9;
					int i;
					for (i = 10, ++j; j < 9; i *= 10, ++j);
					int x = static_cast<int>(*d % i);
					if (x || d + 1 != z) {
						long double round = 2 / LDBL_EPSILON;
						if (((*d / i) & 1) || (i == 1000000000 && d > a && (d[-1] & 1))) {
							round += 2;
						}
						long double small;
						if (x < i / 2) {
							small=0x0.8p0;
						}
						else if (x == i / 2 && d + 1 == z) {
							small=0x1.0p0;
						}
						else {
							small=0x1.8p0;
						}
						if (negative) {
							round *= -1;
							small *= -1;
						}
						*d -= x;
						if (round + small != round) {
							*d = *d + i;
							while (*d > 999999999) {
								*d-- = 0;
								if (d < a) {
									*--a = 0;
								}
								(*d)++;
							}
							exponent = static_cast<int>(9 * (r - a));
							for (i=10; *a >= static_cast<uint32_t>(i); i *= 10, exponent++);
						}
					}

					if (z > d + 1) {
						z = d + 1;
					}
				}

				for (; z > a && !z[-1]; --z);

				if (specific_lower == 'g') {
					if (!precision) {
						++precision;
					}

					if (precision > exponent && exponent >= -4) {
						--specific;
						--specific_lower;
						precision -= exponent + 1;
					}
					else {
						specific -= 2;
						specific_lower -= 2;
						--precision;
					}

					if (!(flags & flags::ALT)) {
						if (z > a && z[-1]) {
							int i;
							for (i = 10, j = 0; z[-1] % i == 0; i *= 10, ++j);
						}
						else {
							j = 9;
						}

						if (specific_lower == 'f') {
							precision = hz::min(
								precision,
								hz::max(static_cast<int>(9 * (z - r - 1) - j), 0));
						}
						else {
							precision = hz::min(
								precision,
								hz::max(static_cast<int>(9 * (z - r - 1) + exponent - j), 0));
						}
					}
				}

				int tmp_len = 1 + precision + (precision || (flags & flags::ALT));
				if (specific_lower == 'f') {
					if (exponent > 0) {
						tmp_len += exponent;
					}
				}
				else {
					e_str = write_int_to_buf(exponent < 0 ? -exponent : exponent, e_buf);
					while (e_buf - e_str < 2) {
						*--e_str = '0';
					}
					*--e_str = (exponent < 0 ? '-' : '+');
					*--e_str = specific;
					tmp_len += e_buf - e_str;
				}

				if (negative) {
					if (!write("-", 1)) {
						return -1;
					}
				}
				else if (flags & flags::SIGN) {
					if (!write("+", 1)) {
						return -1;
					}
				}

				if (specific_lower == 'f') {
					if (a > r) {
						a = r;
					}

					for (d = a; d <= r; ++d) {
						char* str = write_int_to_buf(*d, buf + 9);
						if (d != a) {
							while (str > buf) {
								*--str = '0';
							}
						}
						else if (str == buf + 9) {
							*--str = '0';
						}

						if (!write(str, buf + 9 - str)) {
							return -1;
						}
					}

					if (precision || (flags & flags::ALT)) {
						if (!write(".", 1)) {
							return -1;
						}
					}

					for (; d < z && precision > 0; ++d, precision -= 9) {
						char* str = write_int_to_buf(*d, buf + 9);
						while (str > buf) {
							*--str = '0';
						}

						if (!write(str, hz::min(precision, 9))) {
							return -1;
						}
					}

					if (!pad(precision + 9, 9, '0')) {
						return -1;
					}
				}
				else {
					if (z <= a) {
						z = a + 1;
					}

					for (d = a; d < z && precision >= 0; ++d) {
						char* str = write_int_to_buf(*d, buf + 9);
						if (str == buf + 9) {
							*--str = '0';
						}
						if (d != a) {
							while (str > buf) {
								*--str = '0';
							}
						}
						else {
							if (!write(str++, 1)) {
								return -1;
							}
							if (precision > 0 || (flags & flags::ALT)) {
								if (!write(".", 1)) {
									return -1;
								}
							}
						}

						if (!write(str, hz::min(static_cast<int>(buf + 9 - str), precision))) {
							return -1;
						}

						precision -= static_cast<int>(buf + 9 - str);
					}

					if (!pad(precision + 18, 18, '0')) {
						return -1;
					}

					if (!write(e_str, e_buf - e_str)) {
						return -1;
					}
				}

				// todo space pad

				break;
			}
			case 'u':
			{
				++fmt;
				auto value = pop_unsigned_int();

				IntOpts opts {
					.min_width = has_width ? width : 0,
					.precision = has_precision ? precision : 1,
					.negative = false,
					.always_write_sign = flags & flags::SIGN,
					.plus_as_space = flags & flags::SPACE,
					.pad_with_zero = flags & flags::ZERO,
					.right_pad = flags & flags::LJUST,
					.upper = false,
					.write_prefix = false
				};

				if (!write_int(value, 10, opts)) {
					return -1;
				}

				break;
			}
			case 'x':
			case 'X':
			{
				auto upper = *fmt == 'X';

				++fmt;
				auto value = pop_unsigned_int();

				IntOpts opts {
					.min_width = has_width ? width : 0,
					.precision = has_precision ? precision : 1,
					.negative = false,
					.always_write_sign = flags & flags::SIGN,
					.plus_as_space = flags & flags::SPACE,
					.pad_with_zero = flags & flags::ZERO,
					.right_pad = flags & flags::LJUST,
					.upper = upper,
					.write_prefix = flags & flags::ALT
				};

				if (!write_int(value, 16, opts)) {
					return -1;
				}

				break;
			}
			case 'o':
			{
				++fmt;
				auto value = pop_unsigned_int();

				IntOpts opts {
					.min_width = has_width ? width : 0,
					.precision = has_precision ? precision : 1,
					.negative = false,
					.always_write_sign = flags & flags::SIGN,
					.plus_as_space = flags & flags::SPACE,
					.pad_with_zero = flags & flags::ZERO,
					.right_pad = flags & flags::LJUST,
					.upper = false,
					.write_prefix = flags & flags::ALT
				};

				if (!write_int(value, 8, opts)) {
					return -1;
				}

				break;
			}
			case 'b':
			case 'B':
			{
				auto upper = *fmt == 'B';

				++fmt;
				auto value = pop_unsigned_int();

				IntOpts opts {
					.min_width = has_width ? width : 0,
					.precision = has_precision ? precision : 1,
					.negative = false,
					.always_write_sign = flags & flags::SIGN,
					.plus_as_space = flags & flags::SPACE,
					.pad_with_zero = flags & flags::ZERO,
					.right_pad = flags & flags::LJUST,
					.upper = upper,
					.write_prefix = flags & flags::ALT
				};

				if (!write_int(value, 2, opts)) {
					return -1;
				}

				break;
			}
			case 'p':
			{
				++fmt;

				auto value = reinterpret_cast<uintptr_t>(va_arg(ap, void*));

				IntOpts opts {
					.min_width = 0,
					.precision = 1,
					.negative = false,
					.always_write_sign = false,
					.plus_as_space = false,
					.pad_with_zero = false,
					.right_pad = false,
					.upper = false,
					.write_prefix = true
				};

				if (!write_int(value, 16, opts)) {
					return -1;
				}

				break;
			}
			case 'c':
			{
				++fmt;
				__ensure(flags == 0);

				if (state == State::l) {
					auto c = static_cast<wchar_t>(va_arg(ap, wint_t));
					char buf[4];
					auto res = wcrtomb(buf, c, nullptr);
					if (res == static_cast<size_t>(-1)) {
						buf[0] = '?';
						res = 1;
					}

					if (file) {
						auto tmp = file->write(file, buf, res);
						if (tmp < 0) {
							file->mutex.manual_unlock();
							return -1;
						}
						written += static_cast<size_t>(tmp);
					}
					else {
						written += res;
					}
					if (written > INT_MAX) {
						if (file) {
							file->mutex.manual_unlock();
						}
						errno = EOVERFLOW;
						return -1;
					}
				}
				else {
					__ensure(state == State::None);
					char c = static_cast<char>(va_arg(ap, int));
					if (file) {
						auto tmp = file->write(file, &c, 1);
						if (tmp < 0) {
							file->mutex.manual_unlock();
							return -1;
						}
						written += static_cast<size_t>(tmp);
					}
					else {
						written += 1;
					}
					if (written > INT_MAX) {
						if (file) {
							file->mutex.manual_unlock();
						}
						errno = EOVERFLOW;
						return -1;
					}
				}

				break;
			}
			case 's':
			{
				++fmt;
				// todo
				//__ensure(flags == 0);

				if (state == State::l) {
					const wchar_t* str = va_arg(ap, const wchar_t*);
					if (has_precision) {
						auto tmp = static_cast<size_t>(precision);
						len = 0;
						for (auto* ptr = str; tmp && *ptr; --tmp, ++ptr) {
							++len;
						}
					}
					else {
						len = wcslen(str);
					}
					if (len) {
						char buf[4];
						for (size_t i = 0; i < len; ++i) {
							auto res = wcrtomb(buf, str[i], nullptr);
							if (res == static_cast<size_t>(-1)) {
								buf[0] = '?';
								res = 1;
							}

							if (file) {
								auto tmp = file->write(file, buf, res);
								if (tmp < 0) {
									file->mutex.manual_unlock();
									return -1;
								}
								written += static_cast<size_t>(tmp);
							}
							else {
								written += res;
							}
							if (written > INT_MAX) {
								if (file) {
									file->mutex.manual_unlock();
								}
								errno = EOVERFLOW;
								return -1;
							}
						}
					}
				}
				else {
					__ensure(state == State::None);
					const char* str = va_arg(ap, const char*);
					if (has_precision) {
						auto tmp = static_cast<size_t>(precision);
						len = 0;
						for (auto* ptr = str; tmp && *ptr; --tmp, ++ptr) {
							++len;
						}
					}
					else {
						len = strlen(str);
					}
					if (len) {
						if (file) {
							auto tmp = file->write(file, str, len);
							if (tmp < 0) {
								file->mutex.manual_unlock();
								return -1;
							}
							written += static_cast<size_t>(tmp);
						}
						else {
							written += len;
						}
						if (written > INT_MAX) {
							if (file) {
								file->mutex.manual_unlock();
							}
							errno = EOVERFLOW;
							return -1;
						}
					}
				}

				break;
			}
			case 'n':
			{
				++fmt;

				switch (state) {
					case State::None:
						*va_arg(ap, int*) = static_cast<int>(written);
						break;
					case State::hh:
						*va_arg(ap, signed char*) = static_cast<signed char>(written);
						break;
					case State::h:
						*va_arg(ap, short*) = static_cast<short>(written);
						break;
					case State::l:
						*va_arg(ap, long*) = static_cast<long>(written);
						break;
					case State::ll:
						*va_arg(ap, long long*) = static_cast<long long>(written);
						break;
					case State::j:
						*va_arg(ap, intmax_t*) = static_cast<intmax_t>(written);
						break;
					case State::z:
						*va_arg(ap, ssize_t*) = static_cast<ssize_t>(written);
						break;
					case State::t:
						*va_arg(ap, ptrdiff_t*) = static_cast<ptrdiff_t>(written);
						break;
					case State::L:
						break;
				}

				break;
			}
			default:
			{
				if (file) {
					auto tmp = file->write(file, fmt, 1);
					if (tmp < 0) {
						file->mutex.manual_unlock();
						return -1;
					}
					written += static_cast<size_t>(tmp);
				}
				else {
					written += len;
				}
				if (written > INT_MAX) {
					if (file) {
						file->mutex.manual_unlock();
					}
					errno = EOVERFLOW;
					return -1;
				}
				println(
					"possibly unimplemented printf specifier '",
					hz::string_view {fmt, 1}, "'");
				++fmt;
				break;
			}
		}
	}
}

EXPORT int fprintf(FILE* __restrict file, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	auto ret = vfprintf(file, fmt, ap);
	va_end(ap);
	return ret;
}

EXPORT __attribute__((format(printf, 1, 0))) int vprintf(const char* __restrict fmt, va_list ap) {
	return vfprintf(stdout, fmt, ap);
}

EXPORT int printf(const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	auto ret = vfprintf(stdout, fmt, ap);
	va_end(ap);
	return ret;
}

namespace {
	ssize_t str_file_read(FILE* file, void* data, size_t size) {
		ssize_t unget = 0;
		auto* ptr = static_cast<unsigned char*>(data);
		while (size && file->ungetc_size) {
			auto c = *--file->ungetc_ptr;
			*ptr++ = c;
			--file->ungetc_size;
			--size;
			++unget;
		}
		data = ptr;

		size_t can_copy = hz::min<size_t>(file->read_end - file->read_ptr, size);
		memcpy(data, file->read_ptr, can_copy);
		file->read_ptr += can_copy;
		return static_cast<ssize_t>(can_copy) + unget;
	}
}

EXPORT int vsnprintf(char* __restrict buffer, size_t size, const char* __restrict fmt, va_list ap) {
	FILE str_file {
		.write_base = buffer,
		.write_ptr = buffer,
		.write_end = buffer + size,
		.write = [](FILE* file, const void* data, size_t size) {
			size_t can_copy = hz::min<size_t>(file->write_end - file->write_ptr, size);
			memcpy(file->write_ptr, data, can_copy);
			file->write_ptr += can_copy;
			return static_cast<ssize_t>(size);
		}
	};
	auto ret = vfprintf(&str_file, fmt, ap);
	if (str_file.write_ptr != str_file.write_end) {
		*str_file.write_ptr = 0;
	}
	else if (str_file.write_end != str_file.write_base) {
		*(str_file.write_ptr - 1) = 0;
	}
	return ret;
}

EXPORT int snprintf(char* __restrict buffer, size_t size, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	auto ret = vsnprintf(buffer, size, fmt, ap);
	va_end(ap);
	return ret;
}

EXPORT int vsprintf(char* __restrict buffer, const char* __restrict fmt, va_list ap) {
	return vsnprintf(buffer, SIZE_MAX, fmt, ap);
}

EXPORT int sprintf(char* __restrict buffer, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	auto ret = vsnprintf(buffer, SIZE_MAX, fmt, ap);
	va_end(ap);
	return ret;
}

#define CHECK_CONSUME(cond) if (cond) consume(); else return consumed ? consumed : EOF
#define RETURN return consumed ? consumed : EOF

EXPORT int vfscanf(FILE* __restrict file, const char* __restrict fmt, va_list ap) {
	int consumed = 0;

	auto look_ahead = [&]() {
		char c;
		if (file->read(file, &c, 1) != 1) {
			return static_cast<char>(0);
		}
		ungetc(c, file);
		return c;
	};

	size_t consumed_chars = 0;

	auto consume = [&]() {
		char c;
		file->read(file, &c, 1);
		++consumed_chars;
	};

	auto consume_if_str_equal = [&](hz::string_view str) {
		char buf[32];
		int i = 0;
		for (auto c : str) {
			auto actual = look_ahead();
			if (tolower(actual) == c) {
				consume();
				buf[i++] = actual;
			}
			else {
				for (int j = i; j > 0; --j) {
					__ensure(ungetc(buf[j - 1], file) == buf[j - 1]);
				}
				consumed_chars -= i;
				return false;
			}
		}

		return true;
	};

	for (; *fmt; ++fmt) {
		if (isspace(*fmt)) {
			while (isspace(*fmt)) {
				++fmt;
			}
			--fmt;
			while (isspace(look_ahead())) {
				consume();
			}
			continue;
		}

		if (*fmt != '%') {
			if (look_ahead() == *fmt) {
				consume();
			}
			else {
				if (look_ahead() == 0) {
					return *fmt ? (consumed ? consumed : EOF) : consumed;
				}
				else {
					return consumed;
				}
			}

			continue;
		}
		else if (*fmt == '%' && fmt[1] == '%') {
			CHECK_CONSUME(look_ahead() == '%');
			++fmt;
			continue;
		}

		++fmt;

		bool assign;
		if (*fmt == '*') {
			assign = false;
			++fmt;
		}
		else {
			assign = true;
		}

		int max_width = 0;
		bool has_width = false;
		if (isdigit(*fmt)) {
			size_t count;
			max_width = hz::to_integer<int>(hz::string_view {fmt}, 10, &count);
			fmt += count;
			has_width = true;
		}

		State state = State::None;
		switch (*fmt) {
			case 'h':
				if (fmt[1] == 'h') {
					state = State::hh;
					fmt += 2;
				}
				else {
					state = State::h;
					++fmt;
				}
				break;
			case 'l':
				if (fmt[1] == 'l') {
					state = State::ll;
					fmt += 2;
				}
				else {
					state = State::l;
					++fmt;
				}
				break;
			case 'j':
				state = State::j;
				++fmt;
				break;
			case 'z':
				state = State::z;
				++fmt;
				break;
			case 't':
				state = State::t;
				++fmt;
				break;
			case 'L':
				state = State::L;
				++fmt;
				break;
			default:
				break;
		}

		if (!*fmt) {
			return EOF;
		}

		if (*fmt != '[' && *fmt != 'c' && *fmt != 'n') {
			while (isspace(look_ahead())) {
				consume();
			}
		}

		char* dest = nullptr;
		if (assign) {
			dest = va_arg(ap, char*);
		}
		auto write_to_dest = [&](char c) {
			if (dest) {
				*dest++ = c;
			}
		};
		auto write_to_dest_wchar = [&](wchar_t c) {
			if (dest) {
				auto* ptr = reinterpret_cast<wchar_t*>(dest);
				*ptr++ = c;
				dest = reinterpret_cast<char*>(ptr);
			}
		};

		auto write_int_to_dest = [&](uintmax_t value) {
			if (dest) {
				switch (state) {
					case State::None:
						*reinterpret_cast<unsigned int*>(dest) = value;
						break;
					case State::hh:
						*reinterpret_cast<unsigned char*>(dest) = value;
						break;
					case State::h:
						*reinterpret_cast<unsigned short*>(dest) = value;
						break;
					case State::l:
						*reinterpret_cast<unsigned long*>(dest) = value;
						break;
					case State::ll:
						*reinterpret_cast<unsigned long long*>(dest) = value;
						break;
					case State::j:
						*reinterpret_cast<uintmax_t*>(dest) = value;
						break;
					case State::z:
						*reinterpret_cast<size_t*>(dest) = value;
						break;
					case State::t:
						*reinterpret_cast<ptrdiff_t*>(dest) = static_cast<ptrdiff_t>(value);
						break;
					case State::L:
						panic("invalid vfscanf u state ", static_cast<int>(state));
				}
			}
		};

		switch (*fmt) {
			case 'c':
			{
				if (!has_width) {
					max_width = 1;
				}

				if (state == State::None) {
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c) {
							RETURN;
						}
						consume();
						write_to_dest(c);
					}
				}
				else {
					__ensure(state == State::l);
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c) {
							RETURN;
						}
						consume();
						write_to_dest_wchar(c);
					}
				}

				if (dest) {
					++consumed;
				}
				break;
			}
			case 's':
			{
				if (!has_width) {
					max_width = INT_MAX;
				}

				int used_chars = 0;

				if (state == State::None) {
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c || isspace(c)) {
							break;
						}
						consume();
						write_to_dest(c);
						++used_chars;
					}
					write_to_dest(0);
				}
				else {
					__ensure(state == State::l);
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c || isspace(c)) {
							break;
						}
						consume();
						write_to_dest_wchar(c);
						++used_chars;
					}
					write_to_dest_wchar(0);
				}

				if (dest && used_chars) {
					++consumed;
				}
				else if (!used_chars) {
					if (!consumed && !look_ahead()) {
						return EOF;
					}
				}
				break;
			}
			case '[':
			{
				if (!has_width) {
					max_width = INT_MAX;
				}

				++fmt;

				auto set_start = fmt;
				bool invert = false;
				if (*set_start == '^') {
					invert = true;
					++set_start;
				}

				while (true) {
					if (!*fmt) {
						return -1;
					}

					if (fmt != set_start && *fmt == ']') {
						break;
					}

					++fmt;
				}

				hz::string_view set {set_start, static_cast<size_t>(fmt - set_start)};

				auto set_matches = [&](int c) {
					if (invert) {
						for (size_t i = 0; i < set.size(); ++i) {
							auto set_c = set[i];
							if (set_c == '-' && i != 0 && i + 1 < set.size()) {
								auto start = set_c;
								auto end = set[i + 1];
								++i;
								if (c >= start && c <= end) {
									return false;
								}
							}
							else {
								if (c == set_c) {
									return false;
								}
							}
						}

						return true;
					}
					else {
						for (size_t i = 0; i < set.size(); ++i) {
							auto set_c = set[i];
							if (set_c == '-' && i != 0 && i + 1 < set.size()) {
								auto start = set_c;
								auto end = set[i + 1];
								++i;
								if (c >= start && c <= end) {
									return true;
								}
							}
							else {
								if (c == set_c) {
									return true;
								}
							}
						}

						return false;
					}
				};

				int used_chars = 0;

				if (state == State::None) {
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c || !set_matches(c)) {
							break;
						}
						consume();
						write_to_dest(c);
						++used_chars;
					}
					write_to_dest(0);
				}
				else {
					__ensure(state == State::l);
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c || !set_matches(c)) {
							break;
						}
						consume();
						write_to_dest_wchar(c);
						++used_chars;
					}
					write_to_dest_wchar(0);
				}

				if (dest && used_chars) {
					++consumed;
				}
				else if (!used_chars) {
					if (!consumed && !look_ahead()) {
						return EOF;
					}
				}
				break;
			}
			case 'u':
			case 'd':
			{
				uintmax_t value = 0;

				if (!has_width) {
					max_width = INT_MAX;
				}

				int used_chars = 0;
				int used_extra_chars = 0;

				bool sign = false;
				if (max_width && look_ahead() == '-') {
					consume();
					sign = true;
					--max_width;
					++used_extra_chars;
				}
				else if (max_width && look_ahead() == '+') {
					consume();
					--max_width;
					++used_extra_chars;
				}

				for (int i = 0; i < max_width; ++i) {
					auto c = look_ahead();
					if (!isdigit(c)) {
						break;
					}
					consume();
					value *= 10;
					value += c - '0';
					++used_chars;
				}

				if (sign) {
					value *= -1;
				}

				if (dest && used_chars) {
					write_int_to_dest(value);
					++consumed;
				}
				else if (!used_chars) {
					if (!used_extra_chars && !look_ahead()) {
						return EOF;
					}
					else {
						return consumed;
					}
				}
				break;
			}
			case 'i':
			{
				uintmax_t value = 0;

				if (!has_width) {
					max_width = INT_MAX;
				}

				int base = 10;
				int used_chars = 0;
				int used_extra_chars = 0;

				bool sign = false;
				if (max_width && look_ahead() == '-') {
					consume();
					sign = true;
					--max_width;
					++used_extra_chars;
				}
				else if (max_width && look_ahead() == '+') {
					consume();
					--max_width;
					++used_extra_chars;
				}

				if (max_width && look_ahead() == '0') {
					consume();
					--max_width;
					++used_chars;
					if (look_ahead() == 'x' || look_ahead() == 'X') {
						consume();
						--max_width;
						--used_chars;
						used_extra_chars += 2;
						base = 16;
					}
					else if (look_ahead() == 'b' || look_ahead() == 'B') {
						consume();
						--max_width;
						--used_chars;
						used_extra_chars += 2;
						base = 2;
					}
					else {
						base = 8;
					}
				}

				auto is_proper_digit = [&](int c) {
					if (base != 8) {
						return (c >= '0' && c <= '9') || (c >= 'a' && c <= LOWER_CHARS[base - 1]);
					}
					else {
						return c >= '0' && c <= '7';
					}
				};

				for (int i = 0; i < max_width; ++i) {
					auto c = tolower(look_ahead());
					if (!is_proper_digit(c)) {
						break;
					}
					consume();
					value *= base;
					if (c <= '9') {
						value += c - '0';
					}
					else {
						value += c - 'a' + 10;
					}
					++used_chars;
				}

				if (sign) {
					value *= -1;
				}

				if (dest && used_chars) {
					write_int_to_dest(value);
					++consumed;
				}
				else if (!used_chars) {
					if (!used_extra_chars && !look_ahead()) {
						return EOF;
					}
					else {
						return consumed;
					}
				}
				break;
			}
			case 'o':
			{
				if (!has_width) {
					max_width = INT_MAX;
				}

				int used_chars = 0;
				int used_extra_chars = 0;

				bool sign = false;
				if (max_width && look_ahead() == '-') {
					consume();
					sign = true;
					--max_width;
					++used_extra_chars;
				}
				else if (max_width && look_ahead() == '+') {
					consume();
					--max_width;
					++used_extra_chars;
				}

				if (max_width && look_ahead() == '0') {
					consume();
					--max_width;
					++used_chars;
				}

				uintmax_t value = 0;
				for (int i = 0; i < max_width; ++i) {
					auto c = look_ahead();
					if (c < '0' || c > '7') {
						break;
					}
					consume();
					value *= 8;
					value += c - '0';
					++used_chars;
				}

				if (sign) {
					value *= -1;
				}

				if (dest && used_chars) {
					write_int_to_dest(value);
					++consumed;
				}
				else if (!used_chars) {
					if (!used_extra_chars && !look_ahead()) {
						return EOF;
					}
					else {
						return consumed;
					}
				}
				break;
			}
			case 'x':
			case 'X':
			case 'p':
			{
				if (*fmt == 'p') {
					state = State::l;
				}

				if (!has_width) {
					max_width = INT_MAX;
				}

				int used_chars = 0;
				int used_extra_chars = 0;

				bool sign = false;
				if (max_width && look_ahead() == '-') {
					consume();
					sign = true;
					--max_width;
					++used_extra_chars;
				}
				else if (max_width && look_ahead() == '+') {
					consume();
					--max_width;
					++used_extra_chars;
				}

				if (max_width && look_ahead() == '0') {
					consume();
					--max_width;
					++used_chars;
					if (look_ahead() == 'x' || look_ahead() == 'X') {
						consume();
						--max_width;
						--used_chars;
						used_extra_chars += 2;
					}
				}

				uintmax_t value = 0;
				for (int i = 0; i < max_width; ++i) {
					auto c = look_ahead();
					if (!isxdigit(c)) {
						break;
					}
					consume();
					value *= 16;
					c = static_cast<char>(tolower(c));
					value += c <= '9' ? (c - '0') : (c - 'a' + 10);
					++used_chars;
				}

				if (sign) {
					value *= -1;
				}

				if (dest && used_chars) {
					write_int_to_dest(value);
					++consumed;
				}
				else if (!used_chars) {
					if (!used_extra_chars && !look_ahead()) {
						return EOF;
					}
					else {
						return consumed;
					}
				}
				break;
			}
			case 'n':
			{
				write_int_to_dest(consumed_chars);
				break;
			}
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
			{
				if (!has_width) {
					max_width = INT_MAX;
				}

				int used_extra_chars = 0;

				bool sign = false;
				if (max_width && look_ahead() == '-') {
					consume();
					sign = true;
					--max_width;
					++used_extra_chars;
				}
				else if (max_width && look_ahead() == '+') {
					consume();
					--max_width;
					++used_extra_chars;
				}

				auto write_float_to_dest = [&](long double value) {
					switch (state) {
						case State::None:
							*reinterpret_cast<float*>(dest) = static_cast<float>(value);
							break;
						case State::l:
							*reinterpret_cast<double*>(dest) = static_cast<double>(value);
							break;
						case State::L:
							*reinterpret_cast<long double*>(dest) = value;
							break;
						default:
							panic("unsupported vfscanf f state ", static_cast<int>(state));
					}
				};

				if (consume_if_str_equal("infinity") || consume_if_str_equal("inf")) {
					if (dest) {
						write_float_to_dest(INFINITY);
						++consumed;
					}
					break;
				}
				else if (consume_if_str_equal("nan")) {
					if (dest) {
						write_float_to_dest(NAN);
						++consumed;
					}
					break;
				}

				int used_chars = 0;

				int base = 10;
				if (max_width && look_ahead() == '0') {
					consume();
					--max_width;
					++used_chars;
					if (look_ahead() == 'x' || look_ahead() == 'X') {
						consume();
						--max_width;
						base = 16;
						--used_chars;
						used_extra_chars += 2;
					}
				}

				auto is_proper_digit = [&](int c) {
					if (base != 8) {
						return (c >= '0' && c <= '9') || (c >= 'a' && c <= LOWER_CHARS[base - 1]);
					}
					else {
						return c >= '0' && c <= '7';
					}
				};

				long double value = 0;
				int index = 0;
				for (; index < max_width; ++index) {
					auto c = tolower(look_ahead());
					if (!is_proper_digit(c)) {
						break;
					}
					auto digit = c <= '9' ? (c - '0') : (c - 'a' + 10);
					consume();
					value *= base;
					value += digit;
					++used_chars;
				}

				max_width -= index;

				if (max_width && look_ahead() == '.') {
					consume();
					--max_width;

					long double decimal {1};

					index = 0;
					for (; index < max_width; ++index) {
						auto c = tolower(look_ahead());
						if (!is_proper_digit(c)) {
							break;
						}
						auto digit = c <= '9' ? (c - '0') : (c - 'a' + 10);
						consume();
						decimal /= base;
						value += decimal * digit;
						++used_chars;
					}

					max_width -= index;
				}

				char exponent_char;
				if (base == 10) {
					exponent_char = 'e';
				}
				else {
					exponent_char = 'p';
				}

				if (max_width && tolower(look_ahead()) == exponent_char) {
					consume();
					--max_width;

					bool exponent_sign = false;
					if (max_width && look_ahead() == '-') {
						consume();
						exponent_sign = true;
						--max_width;
					}
					else if (max_width && look_ahead() == '+') {
						consume();
						--max_width;
					}

					int exponent = 0;
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!isdigit(c)) {
							break;
						}
						consume();
						exponent *= 10;
						exponent += c - '0';
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

				if (sign) {
					value *= -1;
				}

				if (dest && used_chars) {
					write_float_to_dest(value);
					++consumed;
				}
				else if (!used_chars) {
					if (!used_extra_chars && !look_ahead()) {
						return EOF;
					}
					else {
						return consumed;
					}
				}
				break;
			}
			default:
				println(
					"possibly unimplemented scanf specifier '",
					hz::string_view {fmt, 1}, "'");
				break;
		}
	}

	return consumed;
}

#undef CHECK_CONSUME
#undef RETURN

EXPORT int fscanf(FILE* __restrict file, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = vfscanf(file, fmt, ap);
	va_end(ap);
	return ret;
}

EXPORT int vsscanf(const char* __restrict str, const char* __restrict fmt, va_list ap) {
	FILE str_file {
		.read_ptr = const_cast<char*>(str),
		.read_end = const_cast<char*>(str + strlen(str)),
		.read_base = const_cast<char*>(str),
		.read = str_file_read
	};
	return vfscanf(&str_file, fmt, ap);
}

EXPORT int sscanf(const char* __restrict str, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = vsscanf(str, fmt, ap);
	va_end(ap);
	return ret;
}

EXPORT int vscanf(const char* __restrict fmt, va_list ap) {
	return vfscanf(stdin, fmt, ap);
}

EXPORT int scanf(const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = vfscanf(stdin, fmt, ap);
	va_end(ap);
	return ret;
}

int parse_file_mode(const char* mode) {
	bool plus = hz::string_view {mode}.contains('+');

	int flags;
	if (*mode == 'r') {
		flags = plus ? O_RDWR : O_RDONLY;
	}
	else if (*mode == 'w') {
		flags = plus ? (O_RDWR | O_CREAT | O_TRUNC) : (O_WRONLY | O_CREAT | O_TRUNC);
	}
	else if (*mode == 'a') {
		flags = plus ? (O_RDWR | O_CREAT | O_APPEND) : (O_WRONLY | O_CREAT | O_APPEND);
	}
	else {
		return -EINVAL;
	}

	++mode;
	while (*mode) {
		if (*mode == 'e') {
			flags |= O_CLOEXEC;
		}
		else if (*mode != '+' && *mode != 'b' && *mode != 't') {
			return -EINVAL;
		}
		++mode;
	}

	return flags;
}

EXPORT FILE* fopen(const char* __restrict filename, const char* __restrict mode) {
	int flags = parse_file_mode(mode);
	if (flags < 0) {
		errno = -flags;
		return nullptr;
	}

	int fd;
	if (auto err = sys_openat(AT_FDCWD, filename, flags, 0666, &fd)) {
		errno = err;
		return nullptr;
	}

	return create_fd_file(fd);
}

EXPORT FILE* freopen(const char* __restrict filename, const char* __restrict mode, FILE* __restrict file) {
	auto guard = file->mutex.lock();

	// todo
	__ensure(filename);

	if (file->fd >= 0) {
		if (auto err = sys_close(file->fd)) {
			delete file;
			errno = err;
			return nullptr;
		}
	}
	else if (file->destroy) {
		file->destroy(file);
	}

	int flags = parse_file_mode(mode);
	if (flags < 0) {
		errno = -flags;
		return nullptr;
	}

	int fd;
	if (auto err = sys_openat(AT_FDCWD, filename, flags, 0666, &fd)) {
		errno = err;
		return nullptr;
	}

	file->fd = fd;

	return file;
}

EXPORT size_t fread(void* __restrict buffer, size_t size, size_t count, FILE* file) {
	auto guard = file->mutex.lock();
	return internal::fread_unlocked(buffer, size, count, file);
}

EXPORT size_t fwrite(const void* __restrict buffer, size_t size, size_t count, FILE* __restrict file) {
	auto guard = file->mutex.lock();
	return internal::fwrite_unlocked(buffer, size, count, file);
}

EXPORT int fgetc(FILE* file) {
	auto guard = file->mutex.lock();
	return internal::fgetc_unlocked(file);
}

EXPORT int getc(FILE* file) {
	return fgetc(file);
}

EXPORT int getchar() {
	return getc(stdin);
}

EXPORT char* fgets(char* __restrict str, int count, FILE* __restrict file) {
	auto lock = file->mutex.lock();
	return internal::fgets_unlocked(str, count, file);
}

EXPORT int ungetc(int ch, FILE* file) {
	auto guard = file->mutex.lock();
	if (file->ungetc_size == static_cast<int>(sizeof(file->ungetc_buffer))) {
		return EOF;
	}
	*file->ungetc_ptr++ = static_cast<unsigned char>(ch);
	++file->ungetc_size;
	return ch;
}

EXPORT int fclose(FILE* file) {
	if (!file) {
		errno = EINVAL;
		return EOF;
	}

	{
		auto guard = file->mutex.lock();

		if (file->no_destroy) {
			return 0;
		}

		{
			if (file->fd >= 0) {
				if (auto err = sys_close(file->fd)) {
					delete file;
					errno = err;
					return EOF;
				}
			}
			else if (file->destroy) {
				file->destroy(file);
			}
		}
	}

	delete file;
	return 0;
}

EXPORT int ferror(FILE* file) {
	auto guard = file->mutex.lock();
	return file->flags & FILE_ERR_FLAG;
}

EXPORT int feof(FILE* file) {
	auto guard = file->mutex.lock();
	return file->flags & FILE_EOF_FLAG;
}

EXPORT void clearerr(FILE* file) {
	auto guard = file->mutex.lock();
	internal::clearerr_unlocked(file);
}

EXPORT int fseek(FILE* file, long offset, int origin) {
	auto guard = file->mutex.lock();
	off64_t tmp;
	if (auto err = sys_lseek(file->fd, offset, origin, &tmp)) {
		errno = err;
		file->flags |= FILE_ERR_FLAG;
		return -1;
	}
	file->ungetc_ptr = file->ungetc_buffer;
	return 0;
}

EXPORT long ftell(FILE* file) {
	auto guard = file->mutex.lock();
	off64_t tmp;
	if (auto err = sys_lseek(file->fd, 0, SEEK_CUR, &tmp)) {
		errno = err;
		file->flags |= FILE_ERR_FLAG;
		return -1;
	}
	if (tmp > LONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}
	return static_cast<long>(tmp);
}

EXPORT void rewind(FILE* file) {
	auto guard = file->mutex.lock();
	off64_t tmp;
	if (auto err = sys_lseek(file->fd, 0, SEEK_SET, &tmp)) {
		errno = err;
	}
	file->flags &= ~(FILE_ERR_FLAG | FILE_EOF_FLAG);
	file->ungetc_ptr = file->ungetc_buffer;
	file->ungetc_size = 0;
}

EXPORT int fsetpos(FILE* __restrict file, const fpos_t* pos) {
	auto guard = file->mutex.lock();

	off64_t tmp;
	if (auto err = sys_lseek(file->fd, pos->__pos, SEEK_SET, &tmp)) {
		errno = err;
		file->flags |= FILE_ERR_FLAG;
		return -1;
	}

	file->ungetc_ptr = file->ungetc_buffer;
	file->ungetc_size = 0;

	return 0;
}

EXPORT int fgetpos(FILE* __restrict file, fpos_t* __restrict pos) {
	auto guard = file->mutex.lock();

	off64_t tmp;
	if (auto err = sys_lseek(file->fd, 0, SEEK_CUR, &tmp)) {
		errno = err;
		file->flags |= FILE_ERR_FLAG;
		return -1;
	}

	if (tmp > LONG_MAX) {
		errno = EOVERFLOW;
		file->flags |= FILE_ERR_FLAG;
		return -1;
	}

	pos->__pos = static_cast<off_t>(tmp);
	return 0;
}

EXPORT int setbuf(
	FILE* file,
	char* __restrict buffer) {
	return 0;
}

EXPORT int setvbuf(
	FILE* file,
	char* __restrict buffer,
	int mode,
	size_t size) {
	return 0;
}

EXPORT int fflush(FILE* file) {
	if (!file) {
		println("fflush flushing all is not implemented");
		return 0;
	}

	if (file->flush) {
		file->flush(file);
	}
	return 0;
}

EXPORT FILE* tmpfile() {
	__ensure(!"tmpfile is not implemented");
}

EXPORT int remove(const char* path) {
	if (auto err = sys_remove(path)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int rename(const char* old_path, const char* new_path) {
	if (auto err = sys_renameat(AT_FDCWD, old_path, AT_FDCWD, new_path)) {
		errno = err;
		return -1;
	}
	return 0;
}

ALIAS(fopen, fopen64);
ALIAS(scanf, __isoc23_scanf);
ALIAS(sscanf, __isoc99_sscanf);
ALIAS(sscanf, __isoc23_sscanf);
ALIAS(vsscanf, __isoc99_vsscanf);
ALIAS(vsscanf, __isoc23_vsscanf);
ALIAS(vfscanf, __isoc23_vfscanf);
ALIAS(fscanf, __isoc99_fscanf);
ALIAS(fscanf, __isoc23_fscanf);
ALIAS(tmpfile, tmpfile64);
ALIAS(putc, _IO_putc);
ALIAS(getc, _IO_getc);
