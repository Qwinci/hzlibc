#include "unistd.h"
#include "utils.hpp"
#include "sys.hpp"

EXPORT void closefrom(int low_fd) {
	if (low_fd < 0) {
		low_fd = 0;
	}
	sys_close_range(low_fd, ~0U, 0);
}
