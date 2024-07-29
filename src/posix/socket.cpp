#include "sys/socket.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int socket(int domain, int type, int protocol) {
	int fd;
	if (auto err = sys_socket(domain, type, protocol, &fd)) {
		errno = err;
		return -1;
	}
	return fd;
}

EXPORT int socketpair(int domain, int type, int protocol, int sv[2]) {
	if (auto err = sys_socketpair(domain, type, protocol, sv)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int connect(int fd, const sockaddr* addr, socklen_t addr_len) {
	if (auto err = sys_connect(fd, addr, addr_len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int bind(int fd, const sockaddr* addr, socklen_t addr_len) {
	if (auto err = sys_bind(fd, addr, addr_len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int listen(int fd, int backlog) {
	if (auto err = sys_listen(fd, backlog)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int accept(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len) {
	int new_fd;
	if (auto err = sys_accept4(fd, addr, addr_len, 0, &new_fd)) {
		errno = err;
		return -1;
	}
	return new_fd;
}

EXPORT int shutdown(int fd, int how) {
	if (auto err = sys_shutdown(fd, how)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getsockopt(int fd, int level, int option, void* __restrict value, socklen_t* __restrict value_len) {
	if (auto err = sys_getsockopt(fd, level, option, value, value_len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int setsockopt(int fd, int level, int option, const void* value, socklen_t value_len) {
	if (auto err = sys_setsockopt(fd, level, option, value, value_len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getsockname(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len) {
	if (auto err = sys_getsockname(fd, addr, addr_len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getpeername(int fd, struct sockaddr* __restrict addr, socklen_t* __restrict addr_len) {
	if (auto err = sys_getpeername(fd, addr, addr_len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT ssize_t send(int fd, const void* buf, size_t len, int flags) {
	return sendto(fd, buf, len, flags, nullptr, 0);
}

EXPORT ssize_t sendto(
	int fd,
	const void* buf,
	size_t len,
	int flags,
	const sockaddr* dest_addr,
	socklen_t addr_len) {
	ssize_t ret;
	if (auto err = sys_sendto(fd, buf, len, flags, dest_addr, addr_len, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t sendmsg(int fd, const msghdr* msg, int flags) {
	ssize_t ret;
	if (auto err = sys_sendmsg(fd, msg, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t recv(int fd, void* __restrict buf, size_t len, int flags) {
	return recvfrom(fd, buf, len, flags, nullptr, nullptr);
}

EXPORT ssize_t recvfrom(
	int fd,
	void* __restrict buf,
	size_t len,
	int flags,
	sockaddr* __restrict src_addr,
	socklen_t* __restrict addr_len) {
	ssize_t ret;
	if (auto err = sys_recvfrom(fd, buf, len, flags, src_addr, addr_len, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t recvmsg(int fd, msghdr* msg, int flags) {
	ssize_t ret;
	if (auto err = sys_recvmsg(fd, msg, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
