#include "err.h"
#include "utils.hpp"
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"

EXPORT void warn(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vwarn(fmt, ap);
	va_end(ap);
}

EXPORT void vwarn(const char* fmt, va_list ap) {
	fprintf(stderr, "%s: ", program_invocation_short_name);
	if (fmt) {
		vfprintf(stderr, fmt, ap);
		fwrite(": ", 1, 2, stderr);
	}
	perror(nullptr);
}

EXPORT void warnx(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vwarnx(fmt, ap);
	va_end(ap);
}

EXPORT void vwarnx(const char* fmt, va_list ap) {
	fprintf(stderr, "%s: ", program_invocation_short_name);
	if (fmt) {
		vfprintf(stderr, fmt, ap);
	}
	putc('\n', stderr);
}

EXPORT __attribute__((__noreturn__)) void err(int status, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	verr(status, fmt, ap);
	va_end(ap);
}

EXPORT __attribute__((__noreturn__)) void verr(int status, const char* fmt, va_list ap) {
	vwarn(fmt, ap);
	exit(status);
}

EXPORT __attribute__((__noreturn__)) void errx(int status, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	verrx(status, fmt, ap);
	va_end(ap);
}

EXPORT __attribute__((__noreturn__)) void verrx(int status, const char* fmt, va_list ap) {
	vwarnx(fmt, ap);
	exit(status);
}
