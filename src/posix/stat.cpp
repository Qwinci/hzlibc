#include "sys/stat.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "fcntl.h"

#if UINTPTR_MAX == UINT32_MAX
#include "limits.h"
#endif

EXPORT int stat(const char* __restrict path, struct stat* __restrict buf) {
#if UINTPTR_MAX == UINT64_MAX
	auto* ptr = reinterpret_cast<struct stat64*>(buf);
	if (auto err = sys_stat(StatTarget::Path, 0, path, 0, ptr)) {
		errno = err;
		return -1;
	}
#else
	struct stat64 large {};
	if (auto err = sys_stat(StatTarget::Path, 0, path, 0, &large)) {
		errno = err;
		return -1;
	}

	if (large.st_size > LONG_MAX || large.st_blocks > LONG_MAX || large.st_ino > ULONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	buf->st_dev = large.st_dev;
	buf->st_ino = static_cast<ino_t>(large.st_ino);
	buf->st_mode = large.st_mode;
	buf->st_nlink = large.st_nlink;
	buf->st_uid = large.st_uid;
	buf->st_gid = large.st_gid;
	buf->st_rdev = large.st_rdev;
	buf->st_size = static_cast<off_t>(large.st_size);
	buf->st_blksize = large.st_blksize;
	buf->st_blocks = static_cast<blkcnt_t>(large.st_blocks);
	buf->st_atim = large.st_atim;
	buf->st_mtim = large.st_mtim;
	buf->st_ctim = large.st_ctim;
#endif

	return 0;
}

EXPORT int stat64(const char* __restrict path, struct stat64* __restrict buf) {
	if (auto err = sys_stat(StatTarget::Path, 0, path, 0, buf)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fstat(int fd, struct stat* buf) {
#if UINTPTR_MAX == UINT64_MAX
	auto* ptr = reinterpret_cast<struct stat64*>(buf);
	if (auto err = sys_stat(StatTarget::Fd, fd, "", 0, ptr)) {
		errno = err;
		return -1;
	}
#else
	struct stat64 large {};
	if (auto err = sys_stat(StatTarget::Fd, fd, "", 0, &large)) {
		errno = err;
		return -1;
	}

	if (large.st_size > LONG_MAX || large.st_blocks > LONG_MAX || large.st_ino > ULONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	buf->st_dev = large.st_dev;
	buf->st_ino = static_cast<ino_t>(large.st_ino);
	buf->st_mode = large.st_mode;
	buf->st_nlink = large.st_nlink;
	buf->st_uid = large.st_uid;
	buf->st_gid = large.st_gid;
	buf->st_rdev = large.st_rdev;
	buf->st_size = static_cast<off_t>(large.st_size);
	buf->st_blksize = large.st_blksize;
	buf->st_blocks = static_cast<blkcnt_t>(large.st_blocks);
	buf->st_atim = large.st_atim;
	buf->st_mtim = large.st_mtim;
	buf->st_ctim = large.st_ctim;
#endif

	return 0;
}

EXPORT int fstat64(int fd, struct stat64* buf) {
	if (auto err = sys_stat(StatTarget::Fd, fd, "", 0, buf)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fstatat(int dir_fd, const char* __restrict path, struct stat* __restrict buf, int flags) {
#if UINTPTR_MAX == UINT64_MAX
	auto* ptr = reinterpret_cast<struct stat64*>(buf);
	if (auto err = sys_stat(StatTarget::FdPath, dir_fd, path, flags, ptr)) {
		errno = err;
		return -1;
	}
#else
	struct stat64 large {};
	if (auto err = sys_stat(StatTarget::FdPath, dir_fd, path, flags, &large)) {
		errno = err;
		return -1;
	}

	if (large.st_size > LONG_MAX || large.st_blocks > LONG_MAX || large.st_ino > ULONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	buf->st_dev = large.st_dev;
	buf->st_ino = static_cast<ino_t>(large.st_ino);
	buf->st_mode = large.st_mode;
	buf->st_nlink = large.st_nlink;
	buf->st_uid = large.st_uid;
	buf->st_gid = large.st_gid;
	buf->st_rdev = large.st_rdev;
	buf->st_size = static_cast<off_t>(large.st_size);
	buf->st_blksize = large.st_blksize;
	buf->st_blocks = static_cast<blkcnt_t>(large.st_blocks);
	buf->st_atim = large.st_atim;
	buf->st_mtim = large.st_mtim;
	buf->st_ctim = large.st_ctim;
#endif

	return 0;
}

EXPORT int fstatat64(int dir_fd, const char* __restrict path, struct stat64* __restrict buf, int flags) {
	if (auto err = sys_stat(StatTarget::FdPath, dir_fd, path, flags, buf)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int lstat(const char* __restrict path, struct stat* __restrict buf) {
#if UINTPTR_MAX == UINT64_MAX
	auto* ptr = reinterpret_cast<struct stat64*>(buf);
	if (auto err = sys_stat(StatTarget::Path, 0, path, AT_SYMLINK_NOFOLLOW, ptr)) {
		errno = err;
		return -1;
	}
#else
	struct stat64 large {};
	if (auto err = sys_stat(StatTarget::Path, 0, path, AT_SYMLINK_NOFOLLOW, &large)) {
		errno = err;
		return -1;
	}

	if (large.st_size > LONG_MAX || large.st_blocks > LONG_MAX || large.st_ino > ULONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	buf->st_dev = large.st_dev;
	buf->st_ino = static_cast<ino_t>(large.st_ino);
	buf->st_mode = large.st_mode;
	buf->st_nlink = large.st_nlink;
	buf->st_uid = large.st_uid;
	buf->st_gid = large.st_gid;
	buf->st_rdev = large.st_rdev;
	buf->st_size = static_cast<off_t>(large.st_size);
	buf->st_blksize = large.st_blksize;
	buf->st_blocks = static_cast<blkcnt_t>(large.st_blocks);
	buf->st_atim = large.st_atim;
	buf->st_mtim = large.st_mtim;
	buf->st_ctim = large.st_ctim;
#endif

	return 0;
}

EXPORT int lstat64(const char* __restrict path, struct stat64* __restrict buf) {
	if (auto err = sys_stat(StatTarget::Path, 0, path, AT_SYMLINK_NOFOLLOW, buf)) {
		errno = err;
		return -1;
	}
	return 0;
}
