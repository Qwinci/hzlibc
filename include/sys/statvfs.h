#ifndef _SYS_STATVFS_H
#define _SYS_STATVFS_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

struct statvfs {
	unsigned long f_bsize;
	unsigned long f_frsize;
	__fsblkcnt_t f_blocks;
	__fsblkcnt_t f_bfree;
	__fsblkcnt_t f_bavail;
	__fsblkcnt_t f_files;
	__fsblkcnt_t f_ffree;
	__fsblkcnt_t f_favail;
	unsigned long f_fsid;
	unsigned long f_flag;
	unsigned long f_namemax;
	unsigned int f_type;
	int __unused[5];
};

struct statvfs64 {
	unsigned long f_bsize;
	unsigned long f_frsize;
	__fsblkcnt64_t f_blocks;
	__fsblkcnt64_t f_bfree;
	__fsblkcnt64_t f_bavail;
	__fsblkcnt64_t f_files;
	__fsblkcnt64_t f_ffree;
	__fsblkcnt64_t f_favail;
	unsigned long f_fsid;
	unsigned long f_flag;
	unsigned long f_namemax;
	unsigned int f_type;
	int __unused[5];
};

int statvfs(const char* __restrict __path, struct statvfs* __restrict __buf);
int statvfs64(const char* __restrict __path, struct statvfs64* __restrict __buf);
int fstatvfs(int __fd, struct statvfs* __buf);
int fstatvfs64(int __fd, struct statvfs64* __buf);

__end_decls

#endif
