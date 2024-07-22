#include "signal.h"
#include "utils.hpp"
#include "errno.h"
#include "sys.hpp"
#include "string.h"

EXPORT int sigprocmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old) {
	if (auto err = pthread_sigmask(how, set, old)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old) {
	if (auto err = sys_sigaction(sig_num, action, old)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int sigemptyset(sigset_t* set) {
	memset(set, 0, sizeof(sigset_t));
	return 0;
}

EXPORT int sigismember(const sigset_t* set, int sig_num) {
	if (sig_num > 1024) {
		errno = EINVAL;
		return -1;
	}
	if (set->__value[sig_num / (8 * sizeof(unsigned long))] & 1UL << (sig_num % (8 * sizeof(unsigned long)))) {
		return 1;
	}
	else {
		return 0;
	}
}

EXPORT int sigaddset(sigset_t* set, int sig_num) {
	if (sig_num > 1024) {
		errno = EINVAL;
		return -1;
	}
	set->__value[sig_num / (8 * sizeof(unsigned long))] |= 1UL << (sig_num % (8 * sizeof(unsigned long)));
	return 0;
}
