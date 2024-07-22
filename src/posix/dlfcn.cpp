#include "dlfcn.h"
#include "utils.hpp"
#include "rtld/rtld.hpp"

EXPORT void* dlopen(const char* filename, int flags) {
	auto return_addr = reinterpret_cast<uintptr_t>(
		__builtin_extract_return_addr(
			__builtin_return_address(0)));

	return __dlapi_open(filename, flags, return_addr);
}

EXPORT int dlclose(void* handle) {
	return __dlapi_close(handle);
}

EXPORT void* dlsym(void* __restrict handle, const char* __restrict symbol) {
	return __dlapi_get_sym(handle, symbol);
}

EXPORT char* dlerror() {
	return __dlapi_get_error();
}
