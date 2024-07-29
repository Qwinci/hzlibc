#include "iconv.h"
#include "utils.hpp"

EXPORT iconv_t iconv_open(const char* to_code, const char* from_code) {
	println(
		"iconv_open from '",
		from_code,
		"' to '",
		to_code,
		"' is not implemented");
	return reinterpret_cast<iconv_t>(-1);
}

EXPORT int iconv_close(iconv_t cd) {
	panic("iconv_close is not implemented");
}

EXPORT size_t iconv(
	iconv_t cd,
	char** __restrict in_buf,
	size_t* __restrict in_bytes_left,
	char** __restrict out_buf,
	size_t* __restrict out_bytes_left) {
	panic("iconv is not implemented");
}
