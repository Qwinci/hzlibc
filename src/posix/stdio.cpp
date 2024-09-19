#include "stdio.h"
#include "utils.hpp"
#include "ansi/stdio_internal.hpp"
#include "sys.hpp"
#include "errno.h"
#include "stdlib.h"
#include "limits.h"
#include "fcntl.h"
#include "string.h"
#include "unistd.h"

EXPORT FILE* fdopen(int fd, const char* mode) {
	return create_fd_file(fd);
}

EXPORT int fileno(FILE* file) {
	return file->fd;
}

EXPORT int fseeko(FILE* file, off_t offset, int whence) {
	auto guard = file->mutex.lock();
	off64_t off;
	if (auto err = sys_lseek(file->fd, offset, whence, &off)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fseeko64(FILE* file, off64_t offset, int whence) {
	auto guard = file->mutex.lock();
	off64_t off;
	if (auto err = sys_lseek(file->fd, offset, whence, &off)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT off_t ftello(FILE* file) {
	auto guard = file->mutex.lock();
	off64_t tmp;
	if (auto err = sys_lseek(file->fd, 0, SEEK_CUR, &tmp)) {
		errno = err;
		return -1;
	}
	if (tmp > LONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}
	return static_cast<off_t>(tmp);
}

EXPORT off64_t ftello64(FILE* file) {
	auto guard = file->mutex.lock();
	off64_t tmp;
	if (auto err = sys_lseek(file->fd, 0, SEEK_CUR, &tmp)) {
		errno = err;
		return -1;
	}
	return tmp;
}

EXPORT int vdprintf(int fd, const char* fmt, va_list ap) {
	FILE file {
		.write = fd_file_write,
		.read = fd_file_read,
		.fd = fd
	};
	return vfprintf(&file, fmt, ap);
}

EXPORT int dprintf(int fd, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = vdprintf(fd, fmt, ap);
	va_end(ap);
	return ret;
}

EXPORT void flockfile(FILE* file) {
	file->mutex.manual_lock();
}

EXPORT void funlockfile(FILE* file) {
	file->mutex.manual_unlock();
}

EXPORT ssize_t getline(char** __restrict line, size_t* __restrict size, FILE* __restrict file) {
	return getdelim(line, size, '\n', file);
}

EXPORT ssize_t getdelim(char** __restrict line, size_t* __restrict size, int delim, FILE* __restrict file) {
	char* buf = *line;
	size_t buf_size;

	if (!buf) {
		buf = static_cast<char*>(malloc(8));
		if (!buf) {
			return -1;
		}
		*line = buf;
		if (size) {
			*size = 8;
		}
		buf_size = 8;
	}
	else {
		if (!size) {
			errno = EINVAL;
			return -1;
		}
		buf_size = *size;
	}

	auto lock = file->mutex.lock();

	size_t count = 0;
	while (true) {
		int c = fgetc_unlocked(file);
		if (c == EOF) {
			errno = EIO;
			return -1;
		}

		if (count + 1 >= buf_size) {
			size_t new_size = buf_size < 512 ? 512 : (buf_size + buf_size / 2);
			if (auto* new_buf = static_cast<char*>(realloc(buf, new_size))) {
				*line = new_buf;
				if (size) {
					*size = new_size;
				}
				buf = new_buf;
				buf_size = new_size;
			}
			else {
				return -1;
			}
		}
		buf[count++] = static_cast<char>(c);

		if (c == delim) {
			buf[count] = 0;
			return static_cast<ssize_t>(count);
		}
	}
}

EXPORT FILE* popen(const char* cmd, const char* type) {
	bool is_write = false;
	if (strchr(type, 'w')) {
		is_write = true;
	}
	if (strchr(type, 'r')) {
		if (is_write) {
			errno = EINVAL;
			return nullptr;
		}
	}
	else if (!is_write) {
		errno = EINVAL;
		return nullptr;
	}

	bool cloexec = strchr(type, 'e');

	struct sigaction new_action {
		.sa_handler = SIG_IGN,
		.sa_mask {},
		.sa_flags {},
		.sa_restorer {}
	};
	struct sigaction old_int_action {};
	struct sigaction old_quit_action {};
	sys_sigaction(SIGINT, &new_action, &old_int_action);
	sys_sigaction(SIGQUIT, &new_action, &old_quit_action);

	sigset_t new_mask {};
	sigset_t old_mask {};
	sigaddset(&new_mask, SIGCHLD);
	sys_sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

	// fd[0] == read
	// fd[1] == write
	int fd[2];
	if (auto err = sys_pipe2(fd, 0)) {
		errno = err;
		return nullptr;
	}

	int parent_fd = is_write ? fd[1] : fd[0];
	int child_fd = is_write ? fd[0] : fd[1];

	pid_t child;
	FILE* ret = nullptr;
	if (auto err = sys_fork(&child)) {
		errno = err;
		sys_close(fd[0]);
		sys_close(fd[1]);
	}
	else if (!child) {
		sys_sigaction(SIGINT, &old_int_action, nullptr);
		sys_sigaction(SIGQUIT, &old_quit_action, nullptr);
		sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);
		sys_close(parent_fd);

		__ensure(sys_dup2(child_fd, is_write ? 0 : 1, 0) == 0);
		sys_close(child_fd);

		const char* args[] {
			"sh", "-c", cmd, nullptr
		};
		sys_execve("/bin/sh", const_cast<char* const*>(args), environ);
		_Exit(127);
	}
	else {
		sys_close(child_fd);

		ret = create_fd_file(parent_fd);
		ret->last_was_read = !is_write;
		ret->data = reinterpret_cast<void*>(static_cast<uintptr_t>(child));

		if (cloexec) {
			fcntl(parent_fd, F_SETFD, O_CLOEXEC);
		}
	}

	sys_sigaction(SIGINT, &old_int_action, nullptr);
	sys_sigaction(SIGQUIT, &old_quit_action, nullptr);
	sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);
	return ret;
}

EXPORT int pclose(FILE* file) {
	auto pid = static_cast<pid_t>(reinterpret_cast<uintptr_t>(file->data));
	if (!pid) {
		fclose(file);
		errno = EINVAL;
		return -1;
	}

	fclose(file);

	int status;
	if (sys_waitpid(pid, &status, 0, nullptr, &pid)) {
		errno = ECHILD;
		return -1;
	}
	return status;
}

struct MemStreamData {
	char* ptr;
	size_t size;
	size_t cap;
	char** user_ptr;
	size_t* user_size_ptr;

	static void flush(FILE* file) {
		auto* data = static_cast<MemStreamData*>(file->data);
	}
};

EXPORT FILE* open_memstream(char** ptr, size_t* size_ptr) {
	__ensure(!"open_memstream is not implemented");
}

EXPORT FILE* fmemopen(void* buf, size_t size, const char* mode) {
	__ensure(!"fmemopen is not implemented");
}

EXPORT char* ctermid(char* str) {
	return str ? strcpy(str, "/dev/tty") : const_cast<char*>("/dev/tty");
}

ALIAS(getdelim, __getdelim);
