#include "error.h"
#include "utils.hpp"
#include "errno.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

EXPORT void error(int status, int err_num, const char* fmt, ...) {
	fflush(stdout);
	// todo use the unlocked versions
	fprintf(stderr, "%s: ", program_invocation_name);
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	if (err_num) {
		fprintf(stderr, ": %s\n", strerror(err_num));
	}
	else {
		fputc('\n', stderr);
	}

	if (status) {
		exit(status);
	}
}
