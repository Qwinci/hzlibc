#include "stdio.h"
#include "utils.hpp"
#include "ansi/stdio_internal.hpp"
#include "sys.hpp"
#include "errno.h"
#include "stdlib.h"

EXPORT FILE* fdopen(int fd, [[maybe_unused]] const char* mode) {
	return create_fd_file(fd);
}

EXPORT int fileno(FILE* file) {
	return file->fd;
}

EXPORT int fseeko(FILE* file, off_t offset, int whence) {
	off64_t off;
	if (auto err = sys_lseek(file->fd, offset, whence, &off)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT ssize_t getline(char** __restrict line, size_t* __restrict size, FILE* __restrict file) {
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
		*buf++ = static_cast<char>(c);
		++count;

		if (c == '\n') {
			*buf = 0;
			return static_cast<ssize_t>(count);
		}
	}
}
