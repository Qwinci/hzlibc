#include "unistd.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "fcntl.h"
#include "limits.h"
#include "sys/ioctl.h"

EXPORT __attribute__((noreturn)) void _exit(int status) {
	sys_exit(status);
}

EXPORT ssize_t read(int fd, void* buf, size_t count) {
	ssize_t ret;
	if (auto err = sys_read(fd, buf, count, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t write(int fd, const void* buf, size_t count) {
	ssize_t ret;
	if (auto err = sys_write(fd, buf, count, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT off_t lseek(int fd, off_t offset, int whence) {
	off64_t ret;
	if (auto err = sys_lseek(fd, offset, whence, &ret)) {
		errno = err;
		return -1;
	}
	return static_cast<off_t>(ret);
}

EXPORT int close(int fd) {
	if (auto err = sys_close(fd)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int pipe(int pipe_fd[2]) {
	if (auto err = sys_pipe2(pipe_fd, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int access(const char* path, int mode) {
	if (auto err = sys_faccessat(AT_FDCWD, path, mode, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int faccessat(int dir_fd, const char* path, int mode, int flags) {
	if (auto err = sys_faccessat(dir_fd, path, mode, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int readlink(const char* __restrict path, char* __restrict buf, size_t buf_size) {
	if (auto err = sys_readlinkat(AT_FDCWD, path, buf, buf_size)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int chown(const char* path, uid_t owner, gid_t group) {
	if (auto err = sys_fchownat(AT_FDCWD, path, owner, group, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fchown(int fd, uid_t owner, gid_t group) {
	if (auto err = sys_fchownat(fd, "", owner, group, AT_EMPTY_PATH)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fchownat(int dir_fd, const char* path, uid_t owner, gid_t group, int flags) {
	if (auto err = sys_fchownat(dir_fd, path, owner, group, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int isatty(int fd) {
	if (auto err = sys_isatty(fd)) {
		errno = err;
		return 0;
	}
	return 1;
}

EXPORT pid_t tcgetpgrp(int fd) {
	pid_t pid;
	if (ioctl(fd, TIOCGPGRP, &pid) < 0) {
		return -1;
	}
	return pid;
}

EXPORT int getentropy(void* buffer, size_t size) {
	if (size > 256) {
		errno = EIO;
		return -1;
	}

	if (auto err = sys_getrandom(buffer, size, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT char* getcwd(char* buf, size_t size) {
	if (!buf) {
		if (!size) {
			size = PATH_MAX;
		}
		buf = static_cast<char*>(malloc(size));
		if (!buf) {
			return nullptr;
		}
	}
	if (auto err = sys_getcwd(buf, size)) {
		errno = err;
		return nullptr;
	}
	return buf;
}

EXPORT int gethostname(char* name, size_t len) {
	if (auto err = sys_gethostname(name, len)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int chroot(const char* path) {
	if (auto err = sys_chroot(path)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int chdir(const char* path) {
	if (auto err = sys_chdir(path)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fchdir(int fd) {
	if (auto err = sys_fchdir(fd)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int setuid(uid_t uid) {
	if (auto err = sys_setuid(uid)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int setgid(gid_t gid) {
	if (auto err = sys_setgid(gid)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getpagesize() {
	return sys_getpagesize();
}

EXPORT int execve(const char* path, char* const argv[], char* const envp[]) {
	if (auto err = sys_execve(path, argv, envp)) {
		errno = err;
		return -1;
	}
	panic("sys_execve returned on success case");
}

EXPORT pid_t fork() {
	pid_t ret;
	if (auto err = sys_fork(&ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

char** environ;
