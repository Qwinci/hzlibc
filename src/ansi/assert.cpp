#include "assert.h"
#include "utils.hpp"
#include "stdlib.h"

EXPORT __attribute__((noreturn)) void __assert_fail(const char* expr, const char* file, unsigned int line, const char* func) {
	println(file, ":", line, ": (", func, "): assertion '", expr, "' failed");
	abort();
}
