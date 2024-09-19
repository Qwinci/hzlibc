#include "regex.h"
#include "utils.hpp"
#include "errno.h"

EXPORT int regcomp(regex_t* reg, const char* regex, int flags) {
	println("regcomp '", regex, "' is not implemented");
	return ENOSYS;
}

EXPORT void regfree(regex_t* reg) {
	panic("regfree is not implemented");
}

EXPORT int regexec(
	const regex_t* __restrict reg,
	const char* __restrict str,
	size_t num_match,
	regmatch_t* __restrict match,
	int flags) {
	panic("regexec is not implemented");
}

EXPORT size_t regerror(
	int err_code,
	const regex_t* regex,
	char* err_buf,
	size_t err_buf_size) {
	panic("regerror is not implemented");
}
