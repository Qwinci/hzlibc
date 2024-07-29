#include "utime.h"
#include "utils.hpp"
#include "sys.hpp"
#include "fcntl.h"
#include "errno.h"

EXPORT int utime(const char* path, const struct utimbuf* times) {
	timespec64 times64[2] {};
	if (times) {
		times64[0].tv_sec = times->actime;
		times64[1].tv_sec = times->modtime;
		if (auto err = sys_utimensat(AT_FDCWD, path, times64, 0)) {
			errno = err;
			return -1;
		}
	}
	else {
		if (auto err = sys_utimensat(AT_FDCWD, path, nullptr, 0)) {
			errno = err;
			return -1;
		}
	}
	return 0;
}
