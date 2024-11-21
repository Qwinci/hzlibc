#include "sys/stat.h"
#include "utils.hpp"

extern "C" EXPORT int __xstat(int ver, const char* __restrict path, struct stat* __restrict buf) {
	return stat(path, buf);
}

extern "C" EXPORT int __lxstat(int ver, const char* __restrict path, struct stat* __restrict buf) {
	return lstat(path, buf);
}

extern "C" EXPORT int __fxstat(int ver, int fd, struct stat* __restrict buf) {
	return fstat(fd, buf);
}

extern "C" EXPORT int __xstat64(int ver, const char* __restrict path, struct stat64* __restrict buf) {
	return stat64(path, buf);
}

extern "C" EXPORT int __lxstat64(int ver, const char* __restrict path, struct stat64* __restrict buf) {
	return lstat64(path, buf);
}

extern "C" EXPORT int __fxstat64(int ver, int fd, struct stat64* __restrict buf) {
	return fstat64(fd, buf);
}

extern "C" EXPORT int __xmknod(int ver, const char* path, mode_t mode, dev_t* dev) {
	return mknod(path, mode, *dev);
}
