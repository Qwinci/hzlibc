#include "sys/klog.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int klogctl(int type, char* buf, int len) {
	int ret;
	if (auto err = sys_klogctl(type, buf, len, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
