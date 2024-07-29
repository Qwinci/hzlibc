#include "sys/poll.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int ppoll(
	struct pollfd* fds,
	nfds_t num_fds,
	const timespec* timeout,
	const sigset_t* sig_mask) {
	int ret;
#if UINTPTR_MAX == UINT64_MAX
	if (auto err = sys_ppoll(fds, num_fds, reinterpret_cast<const timespec64*>(timeout), sig_mask, &ret)) {
		errno = err;
		return -1;
	}
#else
	if (timeout) {
		timespec64 time64 {
			.tv_sec = timeout->tv_sec,
			.tv_nsec = timeout->tv_nsec
		};
		if (auto err = sys_ppoll(fds, num_fds, &time64, sig_mask, &ret)) {
			errno = err;
			return -1;
		}
	}
	else {
		if (auto err = sys_ppoll(fds, num_fds, nullptr, sig_mask, &ret)) {
			errno = err;
			return -1;
		}
	}
#endif
	return ret;
}
