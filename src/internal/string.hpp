#pragma once
#include <bits/mbstate_t.h>
#include <stddef.h>

namespace internal {
	char* strtok_r(char* __restrict str, const char* __restrict delim, char** __restrict save_ptr);
	size_t mbsnrtowcs(
		wchar_t* __restrict dest,
		const char** __restrict src,
		size_t num_chars,
		size_t len,
		mbstate_t* __restrict ps);
}
