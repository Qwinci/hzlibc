#include "sys/utsname.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int uname(struct utsname* buf) {
	if (auto err = sys_uname(buf)) {
		errno = err;
		return -1;
	}
	return 0;
}
