#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <bits/utils.h>
#include <sys/types.h>
#include <time.h>

__begin

#ifdef __x86_64__

struct stat {
	dev_t st_dev;
	ino_t st_ino;
	nlink_t st_nlink;
	mode_t st_mode;
	uid_t st_uid;
	gid_t st_gid;
	unsigned int __pad0;
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long __unused[3];
};

struct stat64 {
	dev_t st_dev;
	ino_t st_ino;
	nlink_t st_nlink;
	mode_t st_mode;
	uid_t st_uid;
	gid_t st_gid;
	unsigned int __pad0;
	dev_t st_rdev;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long __unused[3];
};

#elif defined(__i386__)

struct stat {
	dev_t st_dev;
	unsigned short __pad0;
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	unsigned short __pad1;
	off_t st_size;
	blksize_t st_blksize;
	blkcnt_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	unsigned long __unused[2];
};

struct stat64 {
	dev_t st_dev;
	unsigned int __pad0;
	ino_t __st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	unsigned int __pad1;
	off64_t st_size;
	blksize_t st_blksize;
	blkcnt64_t st_blocks;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	ino64_t st_ino;
};

#else
#error missing architecture specific code
#endif

struct statx_timestamp {
	int64_t tv_sec;
	uint32_t tv_nsec;
	int32_t __reserved;
};

struct statx {
	uint32_t stx_mask;
	uint32_t stx_blksize;
	uint64_t stx_attributes;
	uint32_t stx_nlink;
	uint32_t stx_uid;
	uint32_t stx_gid;
	uint16_t stx_mode;
	uint16_t __pad0;
	uint64_t stx_ino;
	uint64_t stx_size;
	uint64_t stx_blocks;
	uint64_t stx_attributes_mask;
	struct statx_timestamp stx_atime;
	struct statx_timestamp stx_btime;
	struct statx_timestamp stx_ctime;
	struct statx_timestamp stx_mtime;
	uint32_t stx_rdev_major;
	uint32_t stx_rdev_minor;
	uint32_t stx_dev_major;
	uint32_t stx_dev_minor;
	uint64_t stx_mnt_id;
	uint32_t stx_dio_mem_align;
	uint32_t stx_dio_offset_align;
	uint64_t __pad1[12];
};

int stat(const char* __restrict __path, struct stat* __restrict __buf);
int stat64(const char* __restrict __path, struct stat64* __restrict __buf);
int fstat(int __fd, struct stat* __buf);
int fstat64(int __fd, struct stat64* __buf);
int fstatat(int __dir_fd, const char* __restrict __path, struct stat* __restrict __buf, int __flags);
int fstatat64(int __dir_fd, const char* __restrict __path, struct stat64* __restrict __buf, int __flags);
int lstat(const char* __restrict __path, struct stat* __restrict __buf);
int lstat64(const char* __restrict __path, struct stat64* __restrict __buf);

// linux
int statx(
	int __dir_fd,
	const char* __restrict __path,
	int __flags,
	unsigned int __mask,
	struct statx* __restrict __buf);

__end

#endif
