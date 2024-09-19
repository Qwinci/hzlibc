#include "glob.h"
#include "utils.hpp"

EXPORT int glob(
	const char* __restrict pattern,
	int flags,
	int (*err_func)(const char* pat, int err_num),
	glob_t* __restrict g) {
	__ensure(!"glob is not implemented");
}

EXPORT void globfree(glob_t* __restrict g) {
	__ensure(!"globfree is not implemented");
}
