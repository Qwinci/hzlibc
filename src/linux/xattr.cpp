#include "attr/xattr.h"
#include "sys/xattr.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT ssize_t getxattr(const char* path, const char* name, void* value, size_t size) {
	ssize_t ret;
	if (auto err = sys_getxattr(path, name, value, size, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT ssize_t fgetxattr(int fd, const char* name, void* value, size_t size) {
	ssize_t ret;
	if (auto err = sys_fgetxattr(fd, name, value, size, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int setxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
	if (auto err = sys_setxattr(path, name, value, size, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fsetxattr(int fd, const char* name, const void* value, size_t size, int flags) {
	if (auto err = sys_fsetxattr(fd, name, value, size, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int removexattr(const char* path, const char* name) {
	if (auto err = sys_removexattr(path, name)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fremovexattr(int fd, const char* name) {
	if (auto err = sys_fremovexattr(fd, name)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT ssize_t listxattr(const char* path, char* list, size_t size) {
	ssize_t ret;
	if (auto err = sys_listxattr(path, list, size, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
