#include "stdio.h"
#include "utils.hpp"
#include "sys.hpp"
#include "string.h"
#include "stdio_internal.hpp"
#include "fcntl.h"
#include "errno.h"
#include "ctype.h"
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
		return count_read;
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
			width = hz::to_integer<int>(hz::string_view {fmt}, count);
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
				precision = hz::to_integer<int>(hz::string_view {fmt}, count);
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
			{
				++fmt;
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
				break;
			}
			case 'x':
			case 'X':
			{
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

				++fmt;
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

EXPORT int sprintf(char* __restrict buffer, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	auto ret = vsnprintf(buffer, SIZE_MAX, fmt, ap);
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

EXPORT int fclose(FILE* file) {
	if (!file) {
		errno = EINVAL;
		return EOF;
	}

	if (file->no_destroy) {
		return 0;
	}

	{
		auto guard = file->mutex.lock();
		if (auto err = sys_close(file->fd)) {
			delete file;
			errno = err;
			return EOF;
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

EXPORT int fseek(FILE* file, long offset, int origin) {
	auto guard = file->mutex.lock();
	off64_t tmp;
	if (auto err = sys_lseek(file->fd, offset, origin, &tmp)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT void rewind(FILE* file) {
	auto guard = file->mutex.lock();
	off64_t tmp;
	if (auto err = sys_lseek(file->fd, 0, SEEK_SET, &tmp)) {
		errno = err;
	}
	file->flags &= ~(FILE_ERR_FLAG | FILE_EOF_FLAG);
}

EXPORT int setvbuf(
	[[maybe_unused]] FILE* file,
	[[maybe_unused]] char* __restrict buffer,
	[[maybe_unused]] int mode,
	[[maybe_unused]] size_t size) {
	return 0;
}

EXPORT int fflush(FILE*) {
	return 0;
}

ALIAS(fopen, fopen64);
