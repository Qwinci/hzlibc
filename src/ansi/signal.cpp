#include "signal.h"
#include "utils.hpp"
#include "ansi_sys.hpp"
#include "errno.h"

EXPORT sighandler_t signal(int sig_num, sighandler_t handler) {
	struct sigaction act {};
	act.sa_handler = handler;
	act.sa_flags = SA_RESTART;
	struct sigaction old {};
	if (auto err = sys_sigaction(sig_num, &act, &old)) {
		errno = err;
		return SIG_ERR;
	}
	return old.sa_handler;
}

EXPORT int raise(int sig) {
	if (auto err = sys_raise(sig)) {
		errno = err;
		return -1;
	}
	return 0;
}
