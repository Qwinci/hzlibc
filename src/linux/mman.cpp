#include "sys/mman.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int memfd_create(const char* name, unsigned int flags) {
	int ret;
	if (auto err = sys_memfd_create(name, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT void* mremap(void* old_addr, size_t old_size, size_t new_size, int flags, ...) {
	void* new_addr = nullptr;
	if (flags & MREMAP_FIXED) {
		va_list ap;
		va_start(ap, flags);
		new_addr = va_arg(ap, void*);
		va_end(ap);
	}

	void* ret;
	if (auto err = sys_mremap(old_addr, old_size, new_size, flags, new_addr, &ret)) {
		errno = err;
		return MAP_FAILED;
	}
	return ret;
}
