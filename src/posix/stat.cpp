#include "sys/stat.h"
#include "sys/statvfs.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "fcntl.h"

#if UINTPTR_MAX == UINT32_MAX
#include "limits.h"
#endif

#define memcpy __builtin_memcpy

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

EXPORT int statvfs(const char* __restrict path, struct statvfs* __restrict buf) {
	struct statfs s {};
	if (statfs(path, &s)) {
		return -1;
	}
	buf->f_bsize = s.f_bsize;
	buf->f_frsize = s.f_frsize;
	buf->f_blocks = s.f_blocks;
	buf->f_bfree = s.f_bfree;
	buf->f_bavail = s.f_bavail;
	buf->f_files = s.f_files;
	buf->f_ffree = s.f_ffree;
	buf->f_favail = s.f_ffree;
	memcpy(&buf->f_fsid, &s.f_fsid, sizeof(unsigned long));
	buf->f_flag = s.f_flags;
	buf->f_namemax = s.f_namelen;
	buf->f_type = s.f_type;
	return 0;
}

EXPORT int statvfs64(const char* __restrict path, struct statvfs64* __restrict buf) {
	struct statfs64 s {};
	if (statfs64(path, &s)) {
		return -1;
	}
	buf->f_bsize = s.f_bsize;
	buf->f_frsize = s.f_frsize;
	buf->f_blocks = s.f_blocks;
	buf->f_bfree = s.f_bfree;
	buf->f_bavail = s.f_bavail;
	buf->f_files = s.f_files;
	buf->f_ffree = s.f_ffree;
	buf->f_favail = s.f_ffree;
	memcpy(&buf->f_fsid, &s.f_fsid, sizeof(unsigned long));
	buf->f_flag = s.f_flags;
	buf->f_namemax = s.f_namelen;
	buf->f_type = s.f_type;
	return 0;
}

EXPORT int chmod(const char* path, mode_t mode) {
	if (auto err = sys_fchmodat(AT_FDCWD, path, mode, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fchmod(int fd, mode_t mode) {
	if (auto err = sys_fchmodat(fd, "", mode, AT_EMPTY_PATH)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fchmodat(int dir_fd, const char* path, mode_t mode, int flags) {
	if (auto err = sys_fchmodat(dir_fd, path, mode, flags)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int utimensat(int dir_fd, const char* path, const timespec times[2], int flags) {
	if (!path) {
		errno = EINVAL;
		return -1;
	}
#if UINTPTR_MAX == UINT64_MAX
	if (auto err = sys_utimensat(dir_fd, path, reinterpret_cast<const timespec64*>(times), flags)) {
		errno = err;
		return -1;
	}
#else
	timespec64 times64[2] {};
	if (times) {
		times64[0].tv_sec = times[0].tv_sec;
		times64[0].tv_nsec = times[0].tv_nsec;
		times64[1].tv_sec = times[1].tv_sec;
		times64[1].tv_nsec = times[1].tv_nsec;
	}
	if (auto err = sys_utimensat(dir_fd, path, times ? times64 : nullptr, flags)) {
		errno = err;
		return -1;
	}
#endif
	return 0;
}

EXPORT int futimens(int fd, const timespec times[2]) {
	return utimensat(fd, "", times, AT_EMPTY_PATH);
}

EXPORT int mkdir(const char* path, mode_t mode) {
	if (auto err = sys_mkdirat(AT_FDCWD, path, mode)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int mknod(const char* path, mode_t mode, dev_t dev) {
	if (auto err = sys_mknodat(AT_FDCWD, path, mode, dev)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int mkfifoat(int dir_fd, const char* path, mode_t mode) {
	if (auto err = sys_mknodat(dir_fd, path, mode | S_IFIFO, 0)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT mode_t umask(mode_t mask) {
	return sys_umask(mask);
}
