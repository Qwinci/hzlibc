#include "utils.hpp"
#include "stdio.h"

extern "C" EXPORT int __fprintf_chk(FILE* __restrict file, int, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	auto res = vfprintf(file, fmt, ap);
	va_end(ap);
	return res;
}

extern "C" EXPORT int __printf_chk(int, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	auto res = vprintf(fmt, ap);
	va_end(ap);
	return res;
}

extern "C" EXPORT int __sprintf_chk(char* __restrict str, int, size_t, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int res = vsnprintf(str, SIZE_MAX, fmt, ap);
	va_end(ap);
	return res;
}

extern "C" EXPORT int __asprintf_chk(char** __restrict ptr, int, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int res = vasprintf(ptr, fmt, ap);
	va_end(ap);
	return res;
}
