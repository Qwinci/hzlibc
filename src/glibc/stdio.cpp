#include "stdio.h"
#include "utils.hpp"
#include "ansi/stdio_internal.hpp"
#include "errno.h"
#include "string.h"
#include "sys.hpp"
#include "stdlib.h"

extern "C" EXPORT int __overflow(FILE* file, int ch) {
	if (ch != EOF) {
		char c = static_cast<char>(ch);
		if (auto err = file->write(file, &c, 1); err < 0) {
			errno = static_cast<int>(-err);
			return EOF;
		}
	}
	return 0;
}

EXPORT size_t fread_unlocked(void* __restrict buffer, size_t size, size_t count, FILE* __restrict file) {
	if (!size || !count) {
		return 0;
	}
	size_t progress = 0;
	for (; progress < count; ++progress) {
		ssize_t ret = file->read(file, reinterpret_cast<unsigned char*>(buffer) + progress * size, size);
		if (ret < 0 || static_cast<size_t>(ret) < size) {
			return progress;
		}
	}

	return count;
}

EXPORT size_t fwrite_unlocked(const void* __restrict buffer, size_t size, size_t count, FILE* __restrict file) {
	if (!size || !count) {
		return 0;
	}
	ssize_t ret = file->write(file, buffer, size * count);
	if (ret < 0) {
		return 0;
	}
	return static_cast<size_t>(ret);
}

EXPORT int fputc_unlocked(int ch, FILE* file) {
	auto c = static_cast<unsigned char>(ch);
	if (file->write(file, &c, sizeof(c)) < 0) {
		return EOF;
	}
	return ch;
}

EXPORT int fputs_unlocked(const char* __restrict str, FILE* __restrict file) {
	if (file->write(file, str, strlen(str)) < 0) {
		return EOF;
	}
	return 0;
}

EXPORT int fgetc_unlocked(FILE* file) {
	unsigned char c;
	ssize_t count_read;
	if (auto err = sys_read(file->fd, &c, sizeof(c), &count_read)) {
		file->flags |= FILE_ERR_FLAG;
		errno = err;
		return EOF;
	}
	if (count_read != 1) {
		file->flags |= FILE_EOF_FLAG;
		return EOF;
	}
	return c;
}

EXPORT int ferror_unlocked(FILE* file) {
	return file->flags & FILE_ERR_FLAG;
}

EXPORT int feof_unlocked(FILE* file) {
	return file->flags & FILE_EOF_FLAG;
}

EXPORT void clearerr_unlocked(FILE* file) {
	file->flags &= ~FILE_ERR_FLAG;
}

EXPORT int fflush_unlocked(FILE* file) {
	return 0;
}

EXPORT int putc_unlocked(int ch, FILE* file) {
	return fputc_unlocked(ch, file);
}

EXPORT int putchar_unlocked(int ch) {
	return fputc_unlocked(ch, stdout);
}

EXPORT int vasprintf(char** __restrict ptr, const char* fmt, va_list ap) {
	va_list copy;
	va_copy(copy, ap);
	int len = vsnprintf(nullptr, 0, fmt, copy);
	va_end(copy);
	if (len < 0) {
		return len;
	}

	char* buf = static_cast<char*>(malloc(len + 1));
	if (!buf) {
		errno = ENOMEM;
		return -1;
	}
	*ptr = buf;
	return vsnprintf(buf, len + 1, fmt, ap);
}

EXPORT int asprintf(char** __restrict ptr, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int res = vasprintf(ptr, fmt, ap);
	va_end(ap);
	return res;
}
