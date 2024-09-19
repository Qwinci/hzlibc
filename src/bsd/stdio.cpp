#include "stdio.h"
#include "utils.hpp"
#include "ansi/stdio_internal.hpp"
#include "errno.h"

EXPORT int fdclose(FILE* file, int* fd_ptr) {
	if (!file) {
		errno = EINVAL;
		return EOF;
	}

	{
		auto guard = file->mutex.lock();

		if (file->no_destroy) {
			if (fd_ptr) {
				*fd_ptr = file->fd;
			}
			return 0;
		}

		{
			if (file->fd >= 0) {
				if (fd_ptr) {
					*fd_ptr = file->fd;
				}
				delete file;
				return 0;
			}
			else if (file->destroy) {
				file->destroy(file);
			}
		}
	}

	delete file;
	return 0;
}

EXPORT void setlinebuf(FILE* file) {
	setvbuf(file, nullptr, _IOLBF, 0);
}
