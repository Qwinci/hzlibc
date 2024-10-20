#include "sys/select.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int select(
	int num_fds,
	fd_set* __restrict read_fds,
	fd_set* __restrict write_fds,
	fd_set* __restrict except_fds,
	timeval* __restrict timeout) {
	timespec64 timeout64 {};
	if (timeout) {
		timeout64.tv_sec = timeout->tv_sec;
		timeout64.tv_nsec = timeout->tv_usec * 1000;
	}

	int ret;
	if (auto err = sys_pselect(
		num_fds,
		read_fds,
		write_fds, except_fds,
		timeout ? &timeout64 : nullptr,
		nullptr,
		&ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int pselect(
	int num_fds,
	fd_set* __restrict read_fds,
	fd_set* __restrict write_fds,
	fd_set* __restrict except_fds,
	const timespec* __restrict timeout,
	const sigset_t* sig_mask) {
#if UINTPTR_MAX == UINT64_MAX
	int ret;
	if (auto err = sys_pselect(
		num_fds,
		read_fds,
		write_fds,
		except_fds,
		reinterpret_cast<const timespec64*>(timeout),
		sig_mask,
		&ret)) {
		errno = err;
		return -1;
	}
	return ret;
#else
	timespec64 timeout64 {};
	if (timeout) {
		timeout64.tv_sec = timeout->tv_sec;
		timeout64.tv_nsec = timeout->tv_nsec;
	}
	int ret;
	if (auto err = sys_pselect(
		num_fds,
		read_fds,
		write_fds,
		except_fds,
		timeout ? &timeout64 : nullptr,
		sig_mask,
		&ret)) {
		errno = err;
		return -1;
	}
	return ret;
#endif
}
