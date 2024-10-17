#include "stdio.h"
#include "utils.hpp"
#include "ansi/stdio_internal.hpp"
#include "stdio_unlocked.hpp"
#include "errno.h"
#include "string.h"
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
	return internal::fread_unlocked(buffer, size, count, file);
}

EXPORT size_t fwrite_unlocked(const void* __restrict buffer, size_t size, size_t count, FILE* __restrict file) {
	return internal::fwrite_unlocked(buffer, size, count, file);
}

EXPORT int fputc_unlocked(int ch, FILE* file) {
	return internal::fputc_unlocked(ch, file);
}

EXPORT int fputs_unlocked(const char* __restrict str, FILE* __restrict file) {
	return internal::fputs_unlocked(str, file);
}

EXPORT int fgetc_unlocked(FILE* file) {
	return internal::fgetc_unlocked(file);
}

EXPORT int getc_unlocked(FILE* file) {
	return internal::getc_unlocked(file);
}

EXPORT int getchar_unlocked() {
	return internal::getchar_unlocked();
}

EXPORT char* fgets_unlocked(char* __restrict str, int count, FILE* __restrict file) {
	return internal::fgets_unlocked(str, count, file);
}

EXPORT int ferror_unlocked(FILE* file) {
	return internal::ferror_unlocked(file);
}

EXPORT int feof_unlocked(FILE* file) {
	return internal::feof_unlocked(file);
}

EXPORT void clearerr_unlocked(FILE* file) {
	internal::clearerr_unlocked(file);
}

EXPORT int fflush_unlocked(FILE* file) {
	return internal::fflush_unlocked(file);
}

EXPORT int putc_unlocked(int ch, FILE* file) {
	return internal::putc_unlocked(ch, file);
}

EXPORT int putchar_unlocked(int ch) {
	return internal::putchar_unlocked(ch);
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
