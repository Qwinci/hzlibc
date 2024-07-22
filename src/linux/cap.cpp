#include "sys/capability.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int capget(cap_user_header_t hdr, cap_user_data_t data) {
	if (auto err = sys_capget(hdr, data)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int capset(cap_user_header_t hdr, cap_user_data_t data) {
	if (auto err = sys_capset(hdr, data)) {
		errno = err;
		return -1;
	}
	return 0;
}
