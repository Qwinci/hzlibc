#include "utils.hpp"
#include "stdio.h"
#include "syslog.h"
#include "sys/select.h"
#include "string.h"
#include "stdlib.h"
#include "sigjmp.h"
#include "fcntl.h"

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

extern "C" EXPORT int __vasprintf_chk(char** __restrict ptr, int, const char* __restrict fmt, va_list ap) {
	return vasprintf(ptr, fmt, ap);
}

extern "C" EXPORT int __asprintf_chk(char** __restrict ptr, int, const char* __restrict fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int res = vasprintf(ptr, fmt, ap);
	va_end(ap);
	return res;
}

extern "C" EXPORT int __vfprintf_chk(FILE* __restrict file, int, const char* __restrict fmt, va_list ap) {
	return vfprintf(file, fmt, ap);
}

extern "C" EXPORT int __vsprintf_chk(char* __restrict ptr, int, size_t, const char* __restrict fmt, va_list ap) {
	return vsprintf(ptr, fmt, ap);
}

extern "C" EXPORT int __vsnprintf_chk(
	char* __restrict ptr,
	size_t size,
	int,
	size_t,
	const char* __restrict fmt,
	va_list ap) {
	return vsnprintf(ptr, size, fmt, ap);
}

extern "C" EXPORT int __snprintf_chk(
	char* __restrict str,
	size_t size,
	int,
	size_t,
	const char* __restrict fmt,
	...) {
	va_list ap;
	va_start(ap, fmt);
	int res = vsnprintf(str, size, fmt, ap);
	va_end(ap);
	return res;
}

extern "C" EXPORT void __vsyslog_chk(int priority, int, const char* fmt, va_list ap) {
	return vsyslog(priority, fmt, ap);
}

extern "C" EXPORT void __syslog_chk(int priority, int, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsyslog(priority, fmt, ap);
	va_end(ap);
}

extern "C" EXPORT long __fdelt_chk(long fd) {
	return fd / NFDBITS;
}

extern "C" EXPORT void* __memcpy_chk(void* __restrict dest, const void* __restrict src, size_t size, size_t) {
	return memcpy(dest, src, size);
}

extern "C" EXPORT void* __memmove_chk(void* dest, const void* src, size_t size, size_t) {
	return memmove(dest, src, size);
}

extern "C" EXPORT char* __strcpy_chk(char* __restrict dest, const char* __restrict src, size_t) {
	return strcpy(dest, src);
}

extern "C" EXPORT char* __strncpy_chk(char* __restrict dest, const char* __restrict src, size_t count, size_t) {
	return strncpy(dest, src, count);
}

extern "C" EXPORT char* __stpcpy_chk(char* __restrict dest, const char* __restrict src, size_t) {
	return stpcpy(dest, src);
}

extern "C" EXPORT char* __strcat_chk(char* __restrict dest, const char* __restrict src, size_t) {
	return strcat(dest, src);
}

extern "C" EXPORT void __explicit_bzero_chk(void* dest, size_t len, size_t) {
	memset(dest, 0, len);
}

extern "C" EXPORT void* __memset_chk(void* __restrict dest, int ch, size_t size, size_t) {
	return memset(dest, ch, size);
}

extern "C" EXPORT char* __realpath_chk(const char* __restrict path, char* __restrict resolved, size_t) {
	return realpath(path, resolved);
}

extern "C" EXPORT void __longjmp_chk(jmp_buf env, int value) {
	longjmp(env, value);
}

extern "C" EXPORT int __openat_2(int fd, const char* path, int oflag) {
	return openat(fd, path, oflag);
}
