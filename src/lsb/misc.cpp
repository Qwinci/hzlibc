#include "string.h"
#include "utils.hpp"
#include "errno.h"

extern "C" EXPORT int __xpg_strerror_r(int err_num, char* buf, size_t buf_len) {
	char* str = strerror_r(err_num, buf, buf_len);
	if (!str) {
		return errno;
	}

	size_t len = strlen(str);
	if (buf_len < len) {
		return ERANGE;
	}

	memcpy(buf, str, len + 1);
	return 0;
}

extern "C" EXPORT int __libc_current_sigrtmin() {
	// 2 reserved
	return 34;
}

extern "C" EXPORT int __libc_current_sigrtmax() {
	return 64;
}
