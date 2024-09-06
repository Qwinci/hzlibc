#include "sys/io.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int iopl(int level) {
	if (auto err = sys_iopl(level)) {
		errno = err;
		return -1;
	}
	return 0;
}
