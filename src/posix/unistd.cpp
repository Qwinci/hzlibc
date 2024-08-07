#include "unistd.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "fcntl.h"
#include "limits.h"
#include "sys/ioctl.h"
#include "allocator.hpp"
#include <hz/vector.hpp>

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

EXPORT ssize_t pread(int fd, void* buf, size_t count, off_t offset) {
	ssize_t ret;
	if (auto err = sys_pread(fd, buf, count, offset, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t pwrite(int fd, const void* buf, size_t count, off_t offset) {
	ssize_t ret;
	if (auto err = sys_pwrite(fd, buf, count, offset, &ret)) {
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

EXPORT off64_t lseek64(int fd, off64_t offset, int whence) {
	off64_t ret;
	if (auto err = sys_lseek(fd, offset, whence, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
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

EXPORT int dup(int old_fd) {
	int ret;
	if (auto err = sys_dup(old_fd, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int dup2(int old_fd, int new_fd) {
	if (auto err = sys_dup2(old_fd, new_fd, 0)) {
		errno = err;
		return -1;
	}
	return new_fd;
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
	int ret;
	if (auto err = sys_readlinkat(AT_FDCWD, path, buf, buf_size, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
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

EXPORT int link(const char* old_path, const char* new_path) {
	if (auto err = sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int symlink(const char* target, const char* link_path) {
	if (auto err = sys_symlinkat(target, AT_FDCWD, link_path)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int unlink(const char* path) {
	if (auto err = sys_unlinkat(AT_FDCWD, path, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int unlinkat(int dir_fd, const char* path, int flags) {
	if (auto err = sys_unlinkat(dir_fd, path, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int truncate(const char* path, off_t length) {
	if (auto err = sys_truncate(path, length)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int ftruncate(int fd, off_t length) {
	if (auto err = sys_ftruncate(fd, length)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int ftruncate64(int fd, off64_t length) {
	if (auto err = sys_ftruncate(fd, length)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int rmdir(const char* path) {
	if (auto err = sys_rmdir(path)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int sync() {
	if (auto err = sys_sync()) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fsync(int fd) {
	if (auto err = sys_fsync(fd)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int pathconf(const char* path, int name) {
	__ensure(!"pathconf is not implemented");
}

EXPORT int fpathconf(int fd, int name) {
	__ensure(!"fpathconf is not implemented");
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

EXPORT int tcgetattr(int fd, termios* termios) {
	if (ioctl(fd, TCGETS, termios) < 0) {
		return -1;
	}
	return 0;
}

EXPORT int tcsetattr(int fd, int optional_actions, const termios* termios) {
	if (optional_actions == TCSANOW) {
		if (ioctl(fd, TCSETS, termios) < 0) {
			return -1;
		}
	}
	else if (optional_actions == TCSADRAIN) {
		if (ioctl(fd, TCSETSW, termios) < 0) {
			return -1;
		}
	}
	else if (optional_actions == TCSAFLUSH) {
		if (ioctl(fd, TCSETSF, termios) < 0) {
			return -1;
		}
	}
	else {
		errno = EINVAL;
		return -1;
	}
	return 0;
}

EXPORT int tcflush(int fd, int queue_selector) {
	if (ioctl(fd, TCFLSH, queue_selector) < 0) {
		return -1;
	}
	return 0;
}

EXPORT speed_t cfgetospeed(const struct termios* termios) {
	return termios->c_cflag & CBAUD;
}

EXPORT int getentropy(void* buffer, size_t size) {
	if (size > 256) {
		errno = EIO;
		return -1;
	}

	ssize_t tmp;
	if (auto err = sys_getrandom(buffer, size, 0, &tmp)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT long gethostid() {
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

EXPORT pid_t setsid() {
	pid_t ret;
	if (auto err = sys_setsid(&ret)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int setpgid(pid_t pid, pid_t pgid) {
	if (auto err = sys_setpgid(pid, pgid)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT uid_t getuid() {
	return sys_getuid();
}

EXPORT uid_t geteuid() {
	return sys_geteuid();
}

EXPORT gid_t getgid() {
	return sys_getgid();
}

EXPORT gid_t getegid() {
	return sys_getegid();
}

EXPORT pid_t getsid(pid_t pid) {
	pid_t ret;
	if (auto err = sys_getsid(pid, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int getpagesize() {
	return sys_getpagesize();
}

extern "C" int __sched_cpucount(size_t set_size, const cpu_set_t* set);

EXPORT long sysconf(int name) {
	switch (name) {
		case _SC_PAGESIZE:
		{
			return sys_getpagesize();
		}
		case _SC_LOGIN_NAME_MAX:
		{
			return LOGIN_NAME_MAX;
		}
		case _SC_NPROCESSORS_CONF:
		{
			cpu_set_t set {};
			long count = 1;
			if (sys_sched_getaffinity(0, sizeof(set), &set) == 0) {
				count = __sched_cpucount(sizeof(set), &set);
			}
			return count;
		}
		case _SC_PHYS_PAGES:
		{
			struct sysinfo info {};
			if (auto err = sys_sysinfo(&info)) {
				errno = err;
				return -1;
			}
			if (!info.mem_unit) {
				info.mem_unit = 1;
			}
			auto total = static_cast<uint64_t>(info.totalram) * info.mem_unit;
			uint64_t pages = total / sys_getpagesize();
			if (pages > LONG_MAX) {
				return LONG_MAX;
			}
			else {
				return static_cast<long>(pages);
			}
		}
		default:
			panic("sysconf: ", name, " is not implemented");
	}
}

EXPORT int execv(const char* path, char* const argv[]) {
	return execve(path, argv, environ);
}

EXPORT int execve(const char* path, char* const argv[], char* const envp[]) {
	if (auto err = sys_execve(path, argv, envp)) {
		errno = err;
		return -1;
	}
	panic("sys_execve returned on success case");
}

EXPORT int execl(const char* path, const char* arg, ...) {
	va_list ap;
	va_start(ap, arg);

	hz::vector<char*, Allocator> args {Allocator {}};
	args.push_back(const_cast<char*>(arg));
	while (true) {
		auto* arg2 = va_arg(ap, char*);
		args.push_back(arg2);
		if (!arg2) {
			break;
		}
	}
	va_end(ap);
	return execve(path, args.data(), environ);
}

EXPORT int execlp(const char* file, const char* arg, ...) {
	va_list ap;
	va_start(ap, arg);

	hz::vector<char*, Allocator> args {Allocator {}};
	args.push_back(const_cast<char*>(arg));
	while (true) {
		auto* arg2 = va_arg(ap, char*);
		args.push_back(arg2);
		if (!arg2) {
			break;
		}
	}
	va_end(ap);
	return execvp(file, args.data());
}

EXPORT int execvp(const char* file, char* const argv[]) {
	return execvpe(file, argv, environ);
}

void call_atfork_prepare();
void call_atfork_parent();
void call_atfork_child();

EXPORT pid_t fork() {
	call_atfork_prepare();
	pid_t ret;
	if (auto err = sys_fork(&ret)) {
		errno = err;
		return -1;
	}
	if (ret != 0) {
		call_atfork_parent();
	}
	else {
		call_atfork_child();
	}
	return ret;
}

ALIAS(fork, vfork);

EXPORT pid_t getpid() {
	return sys_get_process_id();
}

EXPORT unsigned int sleep(unsigned int seconds) {
	timespec duration {
		.tv_sec = static_cast<time_t>(seconds),
		.tv_nsec = 0
	};
	auto res = nanosleep(&duration, nullptr);
	if (res < 0) {
		return static_cast<unsigned int>(duration.tv_sec);
	}
	return 0;
}

EXPORT int usleep(useconds_t usec) {
	timespec duration {
		.tv_sec = static_cast<time_t>(usec / (1000 * 1000)),
		.tv_nsec = static_cast<long>(usec % (1000 * 1000) * 1000)
	};
	return nanosleep(&duration, nullptr);
}

EXPORT unsigned int alarm(unsigned int seconds) {
	return sys_alarm(seconds);
}

EXPORT int getgroups(size_t size, gid_t* list) {
	int ret;
	if (auto err = sys_getgroups(size, list, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT char** environ;
