#include "sys/io.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

#if defined(__x86_64__) || defined(__i386__)

EXPORT int iopl(int level) {
	if (auto err = sys_iopl(level)) {
		errno = err;
		return -1;
	}
	return 0;
}

#endif
