#include "sys/sendfile.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

#if UINTPTR_MAX == UINT32_MAX
#include <limits.h>
#endif

EXPORT ssize_t sendfile(int out_fd, int in_fd, off_t* offset, size_t count) {
	ssize_t ret;
#if UINTPTR_MAX == UINT64_MAX
	if (auto err = sys_sendfile(out_fd, in_fd, reinterpret_cast<off64_t*>(offset), count, &ret)) {
		errno = err;
		return -1;
	}
#else
	off64_t off;
	if (offset) {
		off = *offset;
	}
	if (auto err = sys_sendfile(out_fd, in_fd, offset ? &off : nullptr, count, &ret)) {
		errno = err;
		return -1;
	}
	if (offset) {
		if (off > LONG_MAX) {
			errno = EOVERFLOW;
			return -1;
		}
		*offset = static_cast<off_t>(off);
	}
#endif
	return ret;
}
