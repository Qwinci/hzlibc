#include "sys/personality.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int personality(unsigned long persona) {
	int ret;
	if (auto err = sys_personality(persona, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
