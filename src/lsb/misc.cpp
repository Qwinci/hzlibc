#include "string.h"
#include "utils.hpp"

extern "C" EXPORT char* __xpg_strerror_r(int err_num, char* buf, size_t buf_len) {
	return strerror_r(err_num, buf, buf_len);
}

extern "C" EXPORT int __libc_current_sigrtmin() {
	// 2 reserved
	return 34;
}

extern "C" EXPORT int __libc_current_sigrtmax() {
	return 64;
}
