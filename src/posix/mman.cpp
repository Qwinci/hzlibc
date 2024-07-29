#include "sys/mman.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
	void* ret;
	if (auto err = sys_mmap(addr, length, prot, flags, fd, offset, &ret)) {
		errno = err;
		return MAP_FAILED;
	}
	return ret;
}

EXPORT void* mmap64(void* addr, size_t length, int prot, int flags, int fd, off64_t offset) {
	void* ret;
	if (auto err = sys_mmap(addr, length, prot, flags, fd, offset, &ret)) {
		errno = err;
		return MAP_FAILED;
	}
	return ret;
}

EXPORT int munmap(void* addr, size_t length) {
	if (auto err = sys_munmap(addr, length)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int mprotect(void* addr, size_t length, int prot) {
	if (auto err = sys_mprotect(addr, length, prot)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int madvise(void* addr, size_t length, int advice) {
	if (auto err = sys_madvise(addr, length, advice)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int mlock(const void* addr, size_t length) {
	if (auto err = sys_mlock(addr, length)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int munlock(const void* addr, size_t length) {
	if (auto err = sys_munlock(addr, length)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int msync(void* addr, size_t length, int flags) {
	if (auto err = sys_msync(addr, length, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int shm_open(const char* name, int oflag, mode_t mode) {
	int ret;
	if (auto err = sys_shm_open(name, oflag, mode, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int shm_unlink(const char* name) {
	if (auto err = sys_shm_unlink(name)) {
		errno = err;
		return -1;
	}
	return 0;
}

ALIAS(madvise, posix_madvise);
