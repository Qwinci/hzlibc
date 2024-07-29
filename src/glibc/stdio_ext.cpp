#include "stdio_ext.h"
#include "utils.hpp"
#include "ansi/stdio_internal.hpp"

EXPORT size_t __fpending(FILE*) {
	return 0;
}

EXPORT int __freading(FILE* file) {
	return file->last_was_read;
}

EXPORT void __fpurge(FILE* file) {

}
