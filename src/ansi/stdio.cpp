#include "stdio.h"
#include "utils.hpp"
#include "sys.hpp"
#include "string.h"
#include "stdio_internal.hpp"
#include "fcntl.h"
#include "errno.h"
#include "ctype.h"
#include "math.h"
#include <hz/new.hpp>
#include <hz/algorithm.hpp>
#include <hz/string_utils.hpp>
#include <limits.h>

namespace {
	ssize_t fd_file_write(FILE* file, const void* data, size_t size) {
		ssize_t written;
		if (auto err = sys_write(file->fd, data, size, &written)) {
			file->error = err;
			return -1;
		}
		file->last_was_read = false;
		return written;
	}

	ssize_t fd_file_read(FILE* file, void* data, size_t size) {
		ssize_t unget = 0;
		auto* ptr = static_cast<unsigned char*>(data);
		while (size && file->ungetc_size) {
			auto c = *file->ungetc_read_ptr++;
			*ptr++ = c;
			if (file->ungetc_read_ptr == file->ungetc_buffer + sizeof(file->ungetc_buffer)) {
				file->ungetc_read_ptr = file->ungetc_buffer;
			}
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

	auto write_int = [&](
		uintmax_t value,
		int base,
		int pad = 0,
		char pad_c = 0,
		bool lower = false) {

		char buf[64];
		char* ptr = buf + 64;
		if (lower) {
			do {
				*--ptr = LOWER_CHARS[value % base];
				value /= base;
			} while (value);
		}
		else {
			do {
				*--ptr = CHARS[value % base];
				value /= base;
			} while (value);
		}

		int len = static_cast<int>((buf + 64) - ptr);
		if (len < pad) {
			pad -= len;
			for (; pad; --pad) {
				*--ptr = pad_c;
			}
		}

		return write(ptr, (buf + 64) - ptr);
	};

	if (file) {
		file->mutex.manual_lock();
	}

	while (true) {
		auto* start = fmt;
		for (; *fmt && *fmt != '%'; ++fmt);
		size_t len = fmt - start;
		for (; *fmt && fmt[1] == '%'; fmt += 2) ++len;
		if (len && !write(start, len)) {
			return -1;
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
			}
			else if (*fmt == '*') {
				++fmt;
				precision = va_arg(ap, int);
				has_precision = true;
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

		switch (*fmt) {
			case 'd':
			case 'i':
			{
				++fmt;
				if (state == State::l) {
					long value = va_arg(ap, long);
					if (value < 0) {
						value *= -1;
						if (!write("-", 1)) {
							return -1;
						}
					}
					else if ((flags & flags::SIGN) && !write("+", 1)) {
						return -1;
					}

					if (has_width && !has_precision) {
						if (!write_int(value, 10, width, (flags & flags::ZERO) ? '0': ' ')) {
							return -1;
						}
					}
					else {
						if (!write_int(value, 10)) {
							return -1;
						}
					}
				}
				else {
					__ensure(state == State::None);

					int value = va_arg(ap, int);
					if (value < 0) {
						value *= -1;
						if (!write("-", 1)) {
							return -1;
						}
					}
					else if ((flags & flags::SIGN) && !write("+", 1)) {
						return -1;
					}

					if (has_width && !has_precision) {
						if (!write_int(value, 10, width, (flags & flags::ZERO) ? '0': ' ')) {
							return -1;
						}
					}
					else {
						if (!write_int(value, 10)) {
							return -1;
						}
					}
				}

				break;
			}
			case 'f':
			case 'g':
			{
				++fmt;
				__ensure(state == State::None);

				double value = va_arg(ap, double);
				if (value < 0) {
					value *= -1;
					if (!write("-", 1)) {
						return -1;
					}
				}
				else if ((flags & flags::SIGN) && !write("+", 1)) {
					return -1;
				}

				if (isinf(value)) {
					if (!write("inf", 3)) {
						return -1;
					}
					break;
				}
				else if (isnan(value)) {
					if (!write("nan", 3)) {
						return -1;
					}
					break;
				}

				auto int_part = static_cast<long long>(value);
				auto dec_part = value - static_cast<double>(int_part);

				if (!write_int(int_part, 10)) {
					return -1;
				}
				if (!has_precision) {
					precision = 6;
				}

				for (int i = 0; i < precision; ++i) {
					dec_part *= 10;
				}

				auto dec_part_int = static_cast<long long>(dec_part);
				if (dec_part_int || (!dec_part_int && (flags & flags::ALT))) {
					if (!write(".", 1)) {
						return -1;
					}
				}

				if (dec_part_int) {
					if (!write_int(dec_part_int, 10)) {
						return -1;
					}
				}
				break;
			}
			case 'u':
			{
				++fmt;
				if (state == State::None) {
					unsigned int value = va_arg(ap, unsigned int);
					if ((flags & flags::SIGN) && !write("+", 1)) {
						return -1;
					}

					if (has_width && !has_precision) {
						if (!write_int(value, 10, width, (flags & flags::ZERO) ? '0': ' ')) {
							return -1;
						}
					}
					else {
						if (!write_int(value, 10)) {
							return -1;
						}
					}
				}
				else if (state == State::l) {
					auto value = va_arg(ap, unsigned long);
					if ((flags & flags::SIGN) && !write("+", 1)) {
						return -1;
					}

					if (has_width && !has_precision) {
						if (!write_int(value, 10, width, (flags & flags::ZERO) ? '0': ' ')) {
							return -1;
						}
					}
					else {
						if (!write_int(value, 10)) {
							return -1;
						}
					}
				}
				else if (state == State::z) {
					size_t value = va_arg(ap, size_t);
					if ((flags & flags::SIGN) && !write("+", 1)) {
						return -1;
					}

					if (has_width && !has_precision) {
						if (!write_int(value, 10, width, (flags & flags::ZERO) ? '0': ' ')) {
							return -1;
						}
					}
					else {
						if (!write_int(value, 10)) {
							return -1;
						}
					}
				}
				else {
					__ensure(false && "unimplemented printf %u state");
				}

				break;
			}
			case 'x':
			case 'X':
			{
				if (state == State::l) {
					unsigned long value = va_arg(ap, unsigned long);
					if ((flags & flags::SIGN) && !write("+", 1)) {
						return -1;
					}
					if (flags & flags::ALT) {
						if (!write("0x", 2)) {
							return -1;
						}
					}

					if (has_width && !has_precision) {
						if (!write_int(value, 16, width, (flags & flags::ZERO) ? '0': ' ', *fmt == 'x')) {
							return -1;
						}
					}
					else {
						if (!write_int(value, 16, 0, 0, *fmt == 'x')) {
							return -1;
						}
					}
				}
				else {
					__ensure(state == State::None);
					unsigned int value = va_arg(ap, int);
					if ((flags & flags::SIGN) && !write("+", 1)) {
						return -1;
					}
					if (flags & flags::ALT) {
						if (!write("0x", 2)) {
							return -1;
						}
					}

					if (has_width && !has_precision) {
						if (!write_int(value, 16, width, (flags & flags::ZERO) ? '0': ' ', *fmt == 'x')) {
							return -1;
						}
					}
					else {
						if (!write_int(value, 16, 0, 0, *fmt == 'x')) {
							return -1;
						}
					}
				}

				++fmt;
				break;
			}
			case 'p':
			{
				++fmt;

				auto value = reinterpret_cast<uintptr_t>(va_arg(ap, void*));
				if ((flags & flags::SIGN) && !write("+", 1)) {
					return -1;
				}
				if (!write("0x", 2)) {
					return -1;
				}

				if (has_width && !has_precision) {
					if (!write_int(value, 16, width, (flags & flags::ZERO) ? '0': ' ', false)) {
						return -1;
					}
				}
				else {
					if (!write_int(value, 16, 0, 0, false)) {
						return -1;
					}
				}

				break;
			}
			case 'c':
			{
				++fmt;
				__ensure(flags == 0);
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
				break;
			}
			case 's':
			{
				++fmt;
				__ensure(flags == 0);
				__ensure(state == State::None);
				const char* str = va_arg(ap, const char*);
				len = strlen(str);
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
			auto c = *file->ungetc_read_ptr++;
			*ptr++ = c;
			if (file->ungetc_read_ptr == file->ungetc_buffer + sizeof(file->ungetc_buffer)) {
				file->ungetc_read_ptr = file->ungetc_buffer;
			}
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
	auto consume = [&]() {
		char c;
		file->read(file, &c, 1);
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
			CHECK_CONSUME(look_ahead() == *fmt);
			continue;
		}
		else if (*fmt == '%' && fmt[1] == '%') {
			CHECK_CONSUME(look_ahead() == '%');
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

		int max_width;
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

		char* dest = nullptr;
		if (assign) {
			dest = va_arg(ap, char*);
		}
		auto write_to_dest = [&](char c) {
			if (dest) {
				*dest++ = c;
			}
		};

		switch (*fmt) {
			case 'c':
			{
				if (!has_width) {
					max_width = 1;
				}
				if (state == State::l) {
					__ensure(!"scanf %lc is not implemented");
				}
				else {
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c) {
							RETURN;
						}
						consume();
						write_to_dest(c);
					}
				}
				++consumed;
				break;
			}
			case 's':
			{
				if (!has_width) {
					max_width = INT_MAX;
				}
				if (state == State::l) {
					panic("scanf %ls is not implemented");
				}
				else {
					for (int i = 0; i < max_width; ++i) {
						auto c = look_ahead();
						if (!c) {
							RETURN;
						}
						else if (isspace(c)) {
							break;
						}
						consume();
						write_to_dest(c);
					}
					write_to_dest(0);
				}
				++consumed;
				break;
			}
			case 'u':
			case 'd':
			{
				__ensure(state == State::None);

				if (!has_width) {
					max_width = INT_MAX;
				}

				bool sign = false;
				if (max_width && look_ahead() == '-') {
					consume();
					sign = true;
					--max_width;
				}
				else if (max_width && look_ahead() == '+') {
					consume();
					--max_width;
				}

				unsigned int value = 0;
				for (int i = 0; i < max_width; ++i) {
					auto c = look_ahead();
					if (!isdigit(c)) {
						break;
					}
					consume();
					value *= 10;
					value += c - '0';
				}

				if (sign) {
					value *= -1;
				}

				if (dest) {
					*reinterpret_cast<unsigned int*>(dest) = value;
				}
				++consumed;
				break;
			}
			case 'x':
			{
				__ensure(state == State::None);

				if (!has_width) {
					max_width = INT_MAX;
				}

				bool sign = false;
				if (max_width && look_ahead() == '-') {
					consume();
					sign = true;
					--max_width;
				}
				else if (max_width && look_ahead() == '+') {
					consume();
					--max_width;
				}

				if (max_width && look_ahead() == '0') {
					consume();
					--max_width;
					if (look_ahead() == 'x' || look_ahead() == 'X') {
						consume();
						--max_width;
					}
				}

				unsigned int value = 0;
				for (int i = 0; i < max_width; ++i) {
					auto c = look_ahead();
					if (!c) {
						RETURN;
					}
					else if (!isxdigit(c)) {
						break;
					}
					consume();
					value *= 16;
					c = static_cast<char>(tolower(c));
					value += c <= '9' ? (c - '0') : (c - 'a' + 10);
				}

				if (sign) {
					value *= -1;
				}

				if (dest) {
					*reinterpret_cast<unsigned int*>(dest) = value;
				}
				++consumed;
				break;
			}
			default:
				println(
					"possibly unimplemented scanf specifier '",
					hz::string_view {fmt, 1}, "'");
				break;
		}
	}

	RETURN;
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
		else if (*mode != '+' && *mode != 'b') {
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
	return fread_unlocked(buffer, size, count, file);
}

EXPORT size_t fwrite(const void* __restrict buffer, size_t size, size_t count, FILE* __restrict file) {
	auto guard = file->mutex.lock();
	return fwrite_unlocked(buffer, size, count, file);
}

EXPORT int fgetc(FILE* file) {
	auto guard = file->mutex.lock();
	return fgetc_unlocked(file);
}

EXPORT int getc(FILE* file) {
	return fgetc(file);
}

EXPORT int getchar() {
	return getc(stdin);
}

EXPORT char* fgets(char* __restrict str, int count, FILE* __restrict file) {
	if (!count) {
		errno = EINVAL;
		return nullptr;
	}

	auto lock = file->mutex.lock();

	for (int i = 0;; ++i) {
		if (i == count - 1) {
			str[i] = 0;
			return str;
		}

		int c = fgetc_unlocked(file);
		if (c == EOF) {
			if (i) {
				str[i] = 0;
				return str;
			}
			else {
				return nullptr;
			}
		}
		str[i] = static_cast<char>(c);

		if (c == '\n') {
			str[i + 1] = 0;
			return str;
		}
	}
}

EXPORT int ungetc(int ch, FILE* file) {
	auto guard = file->mutex.lock();
	if (file->ungetc_size == static_cast<int>(sizeof(file->ungetc_buffer))) {
		return EOF;
	}
	*file->ungetc_ptr++ = static_cast<unsigned char>(ch);
	if (file->ungetc_ptr == file->ungetc_buffer + sizeof(file->ungetc_buffer)) {
		file->ungetc_ptr = file->ungetc_buffer;
	}
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
	clearerr_unlocked(file);
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
	file->ungetc_read_ptr = file->ungetc_buffer;
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
	file->ungetc_read_ptr = file->ungetc_buffer;
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
	file->ungetc_read_ptr = file->ungetc_buffer;
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
	if (file->flush) {
		file->flush(file);
	}
	return 0;
}

EXPORT FILE* tmpfile() {
	__ensure(!"tmpfile is not implemented");
}

EXPORT int remove(const char* path) {
	auto err = sys_unlinkat(AT_FDCWD, path, 0);
	if (err == EISDIR) {
		err = sys_rmdir(path);
	}
	if (err) {
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
ALIAS(vsscanf, __isoc23_vsscanf);
ALIAS(vfscanf, __isoc23_vfscanf);
ALIAS(fscanf, __isoc99_fscanf);
ALIAS(fscanf, __isoc23_fscanf);
ALIAS(tmpfile, tmpfile64);
