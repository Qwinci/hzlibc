#include "sys/ioctl.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include <stdarg.h>

EXPORT int ioctl(int fd, unsigned long op, ...) {
	va_list ap;
	va_start(ap, op);
	void* arg = va_arg(ap, void*);
	int ret;
	if (auto err = sys_ioctl(fd, op, arg, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
