#include "unistd.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "stdlib.h"
#include "allocator.hpp"
#include <hz/string.hpp>

EXPORT int pipe2(int pipe_fd[2], int flags) {
	if (auto err = sys_pipe2(pipe_fd, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int execvpe(const char* file, char* const argv[], char* const envp[]) {
	char* empty[] {
		nullptr
	};

	if (!argv) {
		argv = empty;
	}
	if (!envp) {
		envp = empty;
	}

	hz::string_view file_str {file};
	if (file_str.contains('/')) {
		if (auto err = sys_execve(file, argv, envp)) {
			errno = err;
			return -1;
		}
		panic("sys_execve returned on success case");
	}

	hz::string_view path_env;
	if (auto path_env_str = getenv("PATH")) {
		path_env = path_env_str;
	}
	else {
		path_env = "/bin:/usr/bin";
	}

	size_t offset = 0;
	while (true) {
		auto dir_end = path_env.find(':', offset);
		auto dir = path_env.substr_abs(offset, dir_end);

		hz::string<Allocator> path {Allocator {}};
		path = dir;
		if (!dir.ends_with('/')) {
			path += '/';
		}
		path += file_str;

		auto err = sys_execve(path.data(), argv, envp);
		switch (err) {
			case ENOENT:
			case ENOTDIR:
				break;
			default:
				errno = err;
				return -1;
		}

		if (dir_end == hz::string_view::npos) {
			break;
		}
		else {
			offset = dir_end + 1;
		}
	}

	errno = ENOENT;
	return -1;
}

EXPORT ssize_t copy_file_range(int fd_in, off64_t* off_in, int fd_out, off64_t* off_out, size_t len, unsigned int flags) {
	ssize_t ret;
	if (auto err = sys_copy_file_range(fd_in, off_in, fd_out, off_out, len, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int close_range(unsigned int first, unsigned int last, int flags) {
	if (auto err = sys_close_range(first, last, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getresuid(uid_t* ruid, uid_t* euid, uid_t* suid) {
	if (auto err = sys_getresuid(ruid, euid, suid)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int getresgid(gid_t* rgid, gid_t* egid, gid_t* sgid) {
	if (auto err = sys_getresgid(rgid, egid, sgid)) {
		errno = err;
		return -1;
	}
	return 0;
}
