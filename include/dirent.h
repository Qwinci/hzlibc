#ifndef _DIRENT_H
#define _DIRENT_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

typedef struct __dir DIR;

struct dirent {
	ino_t d_ino;
	off_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[256];
};

struct dirent64 {
	ino64_t d_ino;
	off64_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[256];
};

DIR* opendir(const char* __path);
DIR* fdopendir(int __fd);
struct dirent* readdir(DIR* __dir);
int closedir(DIR* __dir);
int dirfd(DIR* __dir);

__end

#endif
