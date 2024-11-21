#include "dirent.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#if UINTPTR_MAX == UINT32_MAX
#include "limits.h"
#endif

#define memcpy __builtin_memcpy

struct __dir {
	int handle {};
	char buf[2048] {};
	size_t ptr {};
	size_t size {};
	union {
		dirent entry {};
		dirent64 entry64;
	};
};

EXPORT DIR* opendir(const char* path) {
	int handle;
	if (auto err = sys_open_dir(path, &handle)) {
		errno = err;
		return nullptr;
	}

	auto* dir = new DIR {
		.handle = handle,
		.entry {}
	};
	return dir;
}

EXPORT DIR* fdopendir(int fd) {
	return new DIR {
		.handle = fd,
		.entry {}
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

	auto* entry = reinterpret_cast<dirent64*>(dir->buf + dir->ptr);
#if UINTPTR_MAX == UINT64_MAX
	memcpy(&dir->entry, entry, offsetof(dirent64, d_name) + strlen(entry->d_name) + 1);
#else
	if (entry->d_ino > LONG_MAX ||
		entry->d_off > LONG_MAX) {
		errno = EOVERFLOW;
		return nullptr;
	}
	memcpy(&dir->entry.d_ino, &entry->d_ino, 4);
	memcpy(&dir->entry.d_off, &entry->d_off, 4);
	memcpy(&dir->entry.d_reclen, &entry->d_reclen, 2);
	memcpy(&dir->entry.d_type, &entry->d_type, 1);
	memcpy(&dir->entry.d_name, &entry->d_name, strlen(entry->d_name) + 1);
#endif
	dir->ptr += entry->d_reclen;
	return &dir->entry;
}

EXPORT struct dirent64* readdir64(DIR* dir) {
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

	auto* entry = reinterpret_cast<dirent64*>(dir->buf + dir->ptr);
	memcpy(&dir->entry64, entry, offsetof(dirent64, d_name) + strlen(entry->d_name) + 1);
	dir->ptr += entry->d_reclen;
	return &dir->entry64;
}

EXPORT int readdir_r(DIR* dir, dirent* __restrict entry, dirent** __restrict result) {
	__ensure(dir->ptr <= dir->size);
	if (dir->ptr == dir->size) {
		size_t new_size;
		if (auto err = sys_read_dir(dir->handle, dir->buf, sizeof(dir->buf), &new_size)) {
			errno = err;
			return err;
		}
		dir->ptr = 0;
		dir->size = new_size;
		if (!dir->size) {
			*result = nullptr;
			return 0;
		}
	}

	auto* entry_ptr = reinterpret_cast<dirent64*>(dir->buf + dir->ptr);
#if UINTPTR_MAX == UINT64_MAX
	memcpy(entry, entry_ptr, offsetof(dirent64, d_name) + strlen(entry_ptr->d_name) + 1);
#else
	if (entry_ptr->d_ino > LONG_MAX ||
		entry_ptr->d_off > LONG_MAX) {
		errno = EOVERFLOW;
		return EOVERFLOW;
	}
	memcpy(&entry->d_ino, &entry_ptr->d_ino, 4);
	memcpy(&entry->d_off, &entry_ptr->d_off, 4);
	memcpy(&entry->d_reclen, &entry_ptr->d_reclen, 2);
	memcpy(&entry->d_type, &entry_ptr->d_type, 1);
	memcpy(&entry->d_name, &entry_ptr->d_name, strlen(entry_ptr->d_name) + 1);
#endif
	dir->ptr += entry_ptr->d_reclen;
	*result = entry;
	return 0;
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

EXPORT void rewinddir(DIR* dir) {
	off64_t tmp;
	sys_lseek(dir->handle, 0, SEEK_SET, &tmp);
	dir->ptr = 0;
	dir->size = 0;
}

EXPORT int scandir(
	const char* path,
	dirent*** res,
	int (*filter)(const dirent* dirent),
	int (*compar)(const dirent** a, const dirent** b)) {
	DIR* dir = opendir(path);
	if (!dir) {
		return -1;
	}

	int old_errno = errno;
	errno = 0;

	dirent** res_array = nullptr;
	size_t size = 0;
	size_t max = 0;
	while (auto entry = readdir(dir)) {
		if (filter && !filter(entry)) {
			continue;
		}

		if (size == max) {
			max = max < 8 ? 8 : max * 2;
			auto* ptr = static_cast<dirent**>(realloc(res_array, max * sizeof(dirent*)));
			if (!ptr) {
				break;
			}
			res_array = ptr;
		}

		auto& res_entry = res_array[size];
		res_entry = static_cast<dirent*>(malloc(entry->d_reclen));
		if (!res_entry) {
			break;
		}
		memcpy(res_entry, entry, entry->d_reclen);
		++size;
	}

	closedir(dir);

	if (errno) {
		if (res_array) {
			for (; size > 0; --size) {
				free(res_array[size - 1]);
			}
			free(res_array);
		}

		return -1;
	}

	errno = old_errno;

	if (compar) {
		qsort(
			res_array,
			size,
			sizeof(dirent*),
			reinterpret_cast<int (*)(const void*, const void*)>(compar));
	}
	*res = res_array;
	return static_cast<int>(size);
}

#if UINTPTR_MAX == UINT64_MAX

EXPORT [[gnu::alias("scandir")]] int scandir64(
	const char* path,
	struct dirent64*** res,
	int (*filter)(const struct dirent64* dirent),
	int (*compar)(const struct dirent64** a, const struct dirent64** b));

#else

EXPORT int scandir64(
	const char* path,
	dirent64*** res,
	int (*filter)(const dirent64* dirent),
	int (*compar)(const dirent64** a, const dirent64** b)) {
	DIR* dir = opendir(path);
	if (!dir) {
		return -1;
	}

	int old_errno = errno;
	errno = 0;

	dirent64** res_array = nullptr;
	size_t size = 0;
	size_t max = 0;
	while (auto entry = readdir64(dir)) {
		if (filter && !filter(entry)) {
			continue;
		}

		if (size == max) {
			max = max < 8 ? 8 : max * 2;
			auto* ptr = static_cast<dirent64**>(realloc(res_array, max * sizeof(dirent64*)));
			if (!ptr) {
				break;
			}
			res_array = ptr;
		}

		auto& res_entry = res_array[size];
		res_entry = static_cast<dirent64*>(malloc(entry->d_reclen));
		if (!res_entry) {
			break;
		}
		memcpy(res_entry, entry, entry->d_reclen);
		++size;
	}

	closedir(dir);

	if (errno) {
		if (res_array) {
			for (; size > 0; --size) {
				free(res_array[size - 1]);
			}
			free(res_array);
		}

		return -1;
	}

	errno = old_errno;

	if (compar) {
		qsort(
			res_array,
			size,
			sizeof(dirent64*),
			reinterpret_cast<int (*)(const void*, const void*)>(compar));
	}
	*res = res_array;
	return static_cast<int>(size);
}

#endif

EXPORT int alphasort(const struct dirent** a, const struct dirent** b) {
	return strcoll((*a)->d_name, (*b)->d_name);
}

EXPORT int alphasort64(const struct dirent64** a, const struct dirent64** b) {
	return strcoll((*a)->d_name, (*b)->d_name);
}

ALIAS(opendir, opendir64);
