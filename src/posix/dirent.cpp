#include "dirent.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "string.h"

#define memcpy __builtin_memcpy

struct __dir {
	int handle {};
	char buf[2048] {};
	size_t ptr {};
	size_t size {};
	dirent entry {};
};

EXPORT DIR* opendir(const char* path) {
	int handle;
	if (auto err = sys_open_dir(path, &handle)) {
		errno = err;
		return nullptr;
	}

	auto* dir = new DIR {
		.handle = handle,
	};
	return dir;
}

EXPORT DIR* fdopendir(int fd) {
	return new DIR {
		.handle = fd
	};
}

EXPORT dirent* readdir(DIR* dir) {
	__ensure(dir->ptr <= dir->size);
	if (dir->ptr == dir->size) {
		size_t new_size;
		if (auto err = sys_read_dir(dir->handle, dir->buf, sizeof(dir->buf), &new_size)) {
			errno = err;
			return nullptr;
		}
		dir->ptr = 0;
		dir->size = new_size;
		if (!dir->size) {
			return nullptr;
		}
	}

	auto* entry = reinterpret_cast<dirent*>(dir->buf + dir->ptr);
	memcpy(&dir->entry, entry, offsetof(dirent, d_name) + strlen(entry->d_name) + 1);
	dir->ptr += entry->d_reclen;
	return &dir->entry;
}

EXPORT int closedir(DIR* dir) {
	if (auto err = sys_close(dir->handle)) {
		errno = err;
		delete dir;
		return -1;
	}
	delete dir;
	return 0;
}

EXPORT int dirfd(DIR* dir) {
	return dir->handle;
}
