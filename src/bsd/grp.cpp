#include "grp.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int setgroups(size_t size, const gid_t* list) {
	if (auto err = sys_setgroups(size, list)) {
		errno = err;
		return -1;
	}
	return 0;
}
