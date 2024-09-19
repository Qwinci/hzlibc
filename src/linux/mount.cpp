#include "sys/mount.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int mount(const char* source, const char* target, const char* fs_type, unsigned long flags, const void* data) {
	if (auto err = sys_mount(source, target, fs_type, flags, data)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int umount(const char* target) {
	if (auto err = sys_umount2(target, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int umount2(const char* target, int flags) {
	if (auto err = sys_umount2(target, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}
