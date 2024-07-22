#include "unistd.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int pipe2(int pipe_fd[2], int flags) {
	if (auto err = sys_pipe2(pipe_fd, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int execvpe(const char* file, char* const argv[], char* const envp[]) {
	panic("execvpe is not implemented");
}

EXPORT ssize_t copy_file_range(int fd_in, off64_t* off_in, int fd_out, off64_t* off_out, size_t len, unsigned int flags) {
	ssize_t ret;
	if (auto err = sys_copy_file_range(fd_in, off_in, fd_out, off_out, len, flags, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
