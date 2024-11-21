#include "utils.hpp"
#include "features.h"

EXPORT char __libc_single_threaded = 0;

#define STRINGIFY_HELPER(value) #value
#define STRINGIFY(value) STRINGIFY_HELPER(value)

extern "C" EXPORT const char* gnu_get_libc_version() {
	return STRINGIFY(__GLIBC__) "." STRINGIFY(__GLIBC_MINOR__);
}
