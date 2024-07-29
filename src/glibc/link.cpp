#include "link.h"
#include "utils.hpp"
#include "rtld/rtld.hpp"

EXPORT int dl_iterate_phdr(
	int (*callback)(
		struct dl_phdr_info* info,
		size_t size,
		void* data),
	void* data) {
	return __dlapi_iterate_phdr(callback, data);
}
