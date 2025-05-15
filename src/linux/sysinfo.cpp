#include "sys/sysinfo.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int sysinfo(struct sysinfo* info) {
	if (int err = sys_sysinfo(info)) {
		errno = err;
		return -1;
	}
	return 0;
}
