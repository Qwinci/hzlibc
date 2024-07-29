#include "sys/wait.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT pid_t wait(int* status) {
	pid_t ret;
	if (auto err = sys_waitpid(-1, status, 0, nullptr, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT pid_t waitpid(pid_t pid, int* status, int options) {
	pid_t ret;
	if (auto err = sys_waitpid(pid, status, options, nullptr, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
