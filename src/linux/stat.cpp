#include "sys/stat.h"
#include "sys/statfs.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

#if UINTPTR_MAX == UINT32_MAX
#include <limits.h>
#endif

EXPORT int statx(int dir_fd, const char* __restrict path, int flags, unsigned int mask, struct statx* __restrict buf) {
	if (auto err = sys_statx(dir_fd, path, flags, mask, buf)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int statfs(const char* path, struct statfs* buf) {
#if UINTPTR_MAX == UINT64_MAX
	auto* ptr = reinterpret_cast<struct statfs64*>(buf);
	if (auto err = sys_statfs(path, ptr)) {
		errno = err;
		return -1;
	}
#else
	struct statfs64 large {};
	if (auto err = sys_statfs(path, &large)) {
		errno = err;
		return -1;
	}

	if (large.f_blocks > ULONG_MAX || large.f_bfree > ULONG_MAX || large.f_bavail > ULONG_MAX ||
	    large.f_files > ULONG_MAX || large.f_ffree > ULONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	buf->f_type = large.f_type;
	buf->f_bsize = large.f_bsize;
	buf->f_blocks = large.f_blocks;
	buf->f_bfree = static_cast<__fsblkcnt_t>(large.f_bfree);
	buf->f_bavail = static_cast<__fsblkcnt_t>(large.f_bavail);
	buf->f_files = static_cast<__fsfilcnt_t>(large.f_files);
	buf->f_ffree = static_cast<__fsfilcnt_t>(large.f_ffree);
	buf->f_fsid = large.f_fsid;
	buf->f_namelen = large.f_namelen;
	buf->f_frsize = large.f_frsize;
	buf->f_flags = large.f_flags;
#endif

	return 0;
}

EXPORT int statfs64(const char* path, struct statfs64* buf) {
	if (auto err = sys_statfs(path, buf)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int fstatfs(int fd, struct statfs* buf) {
#if UINTPTR_MAX == UINT64_MAX
	auto* ptr = reinterpret_cast<struct statfs64*>(buf);
	if (auto err = sys_fstatfs(fd, ptr)) {
		errno = err;
		return -1;
	}
#else
	struct statfs64 large {};
	if (auto err = sys_fstatfs(fd, &large)) {
		errno = err;
		return -1;
	}

	if (large.f_blocks > ULONG_MAX || large.f_bfree > ULONG_MAX || large.f_bavail > ULONG_MAX ||
		large.f_files > ULONG_MAX || large.f_ffree > ULONG_MAX) {
		errno = EOVERFLOW;
		return -1;
	}

	buf->f_type = large.f_type;
	buf->f_bsize = large.f_bsize;
	buf->f_blocks = large.f_blocks;
	buf->f_bfree = static_cast<__fsblkcnt_t>(large.f_bfree);
	buf->f_bavail = static_cast<__fsblkcnt_t>(large.f_bavail);
	buf->f_files = static_cast<__fsfilcnt_t>(large.f_files);
	buf->f_ffree = static_cast<__fsfilcnt_t>(large.f_ffree);
	buf->f_fsid = large.f_fsid;
	buf->f_namelen = large.f_namelen;
	buf->f_frsize = large.f_frsize;
	buf->f_flags = large.f_flags;
#endif

	return 0;
}

EXPORT int fstatfs64(int fd, struct statfs64* buf) {
	if (auto err = sys_fstatfs(fd, buf)) {
		errno = err;
		return -1;
	}
	return 0;
}
