#include "sys/wait.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT pid_t wait4(pid_t pid, int* status, int options, struct rusage* rusage) {
	pid_t ret;
	if (auto err = sys_waitpid(pid, status, options, rusage, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
