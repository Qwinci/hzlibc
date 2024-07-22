#ifndef _SYS_STATFS_H
#define _SYS_STATFS_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

struct statfs {
	__fsword_t f_type;
	__fsword_t f_bsize;
	__fsblkcnt_t f_blocks;
	__fsblkcnt_t f_bfree;
	__fsblkcnt_t f_bavail;
	__fsfilcnt_t f_files;
	__fsfilcnt_t f_ffree;
	__fsid_t f_fsid;
	__fsword_t f_namelen;
	__fsword_t f_frsize;
	__fsword_t f_flags;
	__fsword_t f_spare[4];
};

struct statfs64 {
	__fsword_t f_type;
	__fsword_t f_bsize;
	__fsblkcnt64_t f_blocks;
	__fsblkcnt64_t f_bfree;
	__fsblkcnt64_t f_bavail;
	__fsfilcnt64_t f_files;
	__fsfilcnt64_t f_ffree;
	__fsid_t f_fsid;
	__fsword_t f_namelen;
	__fsword_t f_frsize;
	__fsword_t f_flags;
	__fsword_t f_spare[4];
};

int statfs(const char* __path, struct statfs* __buf);
int statfs64(const char* __path, struct statfs64* __buf);
int fstatfs(int __fd, struct statfs* __buf);
int fstatfs64(int __fd, struct statfs64* __buf);

__end

#endif
