#ifndef _MNTENT_H
#define _MNTENT_H

#include <bits/utils.h>
#include <stdio.h>

__begin_decls

struct mntent {
	char* mnt_fsname;
	char* mnt_dir;
	char* mnt_type;
	char* mnt_opts;
	int mnt_freq;
	int mnt_passno;
};

FILE* setmntent(const char* __file_name, const char* __type);
int endmntent(FILE* __file);
char* hasmntopt(const struct mntent* __restrict __mnt, const char* __restrict __option);

// glibc
struct mntent* getmntent_r(
	FILE* __restrict __file,
	struct mntent* __restrict __mnt_buf,
	char* __restrict __buf,
	int __buf_len);

__end_decls

#endif
