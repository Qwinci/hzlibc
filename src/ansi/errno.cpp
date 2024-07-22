#include "errno.h"
#include "utils.hpp"

#undef errno

thread_local int __errno = 0;

EXPORT int* __errno_location() {
	return &__errno;
}
