#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <bits/utils.h>
#include <sys/types.h>
#include <time.h>

__begin_decls

#define S_IFMT 0xF000
#define S_IFIFO 0x1000
#define S_IFCHR 0x2000
#define S_IFDIR 0x4000
#define S_IFBLK 0x6000
#define S_IFREQ 0x8000
#define S_IFLNK 0xA000
#define S_IFSOCK 0xC000

#define S_IEXEC 0x40
#define S_IWRITE 0x80
#define S_IREAD 0x100
#define S_ISVTX 0x200
#define S_ISGID 0x400
#define S_ISUID 0x800

#define UTIME_NOW ((1L << 30) - 1L)
#define UTIME_OMIT ((1L << 30) - 2L)

#define __S_ISTYPE(mode, mask) (((mode) & S_IFMT) == (mask))
#define S_ISFIFO(mode) __S_ISTYPE((mode), S_IFIFO)
#define S_ISCHR(mode) __S_ISTYPE((mode), S_IFCHR)
#define S_ISDIR(mode) __S_ISTYPE((mode), S_IFDIR)
#define S_ISBLK(mode) __S_ISTYPE((mode), S_IFBLK)
#define S_ISREQ(mode) __S_ISTYPE((mode), S_IFREQ)
#define S_ISLNK(mode) __S_ISTYPE((mode), S_IFLNK)
#define S_ISSOCK(mode) __S_ISTYPE((mode), S_IFSOCK)

#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#define S_IRWXU (S_IREAD | S_IWRITE | S_IEXEC)

#define S_IRGRP (S_IRUSR >> 3)
#define S_IWGRP (S_IWUSR >> 3)
#define S_IXGRP (S_IXUSR >> 3)
#define S_IRWXG (S_IRWXU >> 3)

#define S_IROTH (S_IRGRP >> 3)
#define S_IWOTH (S_IWGRP >> 3)
#define S_IXOTH (S_IXGRP >> 3)
#define S_IRWXO (S_IRWXG >> 3)

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

#ifdef __USE_XOPEN2K8
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
#else
	time_t st_atime;
	unsigned long st_atimensec;
	time_t st_mtime;
	unsigned long st_mtimensec;
	time_t st_ctime;
	unsigned long st_ctimensec;
#endif

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

#ifdef __USE_XOPEN2K8
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
#else
	time_t st_atime;
	unsigned long st_atimensec;
	time_t st_mtime;
	unsigned long st_mtimensec;
	time_t st_ctime;
	unsigned long st_ctimensec;
#endif

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

#ifdef __USE_XOPEN2K8
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
#else
	time_t st_atime;
	unsigned long st_atimensec;
	time_t st_mtime;
	unsigned long st_mtimensec;
	time_t st_ctime;
	unsigned long st_ctimensec;
#endif

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

#ifdef __USE_XOPEN2K8
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
#else
	time_t st_atime;
	unsigned long st_atimensec;
	time_t st_mtime;
	unsigned long st_mtimensec;
	time_t st_ctime;
	unsigned long st_ctimensec;
#endif

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

int chmod(const char* __path, mode_t __mode);
int fchmod(int __fd, mode_t __mode);
int fchmodat(int __dir_fd, const char* __path, mode_t __mode, int __flags);
int utimensat(int __dir_fd, const char* __path, const struct timespec __times[2], int __flags);
int futimens(int __fd, const struct timespec __times[2]);
int mkdir(const char* __path, mode_t __mode);
int mknod(const char* __path, mode_t __mode, dev_t __dev);
int mkfifoat(int __dir_fd, const char* __path, mode_t __mode);

mode_t umask(mode_t __mask);

// linux
int statx(
	int __dir_fd,
	const char* __restrict __path,
	int __flags,
	unsigned int __mask,
	struct statx* __restrict __buf);

__end_decls

#endif
