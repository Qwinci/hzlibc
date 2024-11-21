#ifndef _DIRENT_H
#define _DIRENT_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

#define DT_UNKNOWN 0
#define DT_FIFO 1
#define DT_CHR 2
#define DT_DIR 4
#define DT_BLK 6
#define DT_REG 8
#define DT_LNK 10
#define DT_SOCK 12
#define DT_WHT 14

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
DIR* opendir64(const char* __path);
DIR* fdopendir(int __fd);
struct dirent* readdir(DIR* __dir);
struct dirent64* readdir64(DIR* __dir);
__attribute__((__deprecated__("use readdir instead")))
int readdir_r(DIR* __dir, struct dirent* __restrict __entry, struct dirent** __restrict __result);
int closedir(DIR* __dir);
void rewinddir(DIR* __dir);
int dirfd(DIR* __dir);

int scandir(
	const char* __path,
	struct dirent*** __res,
	int (*__filter)(const struct dirent* __dirent),
	int (*__compar)(const struct dirent** __a, const struct dirent** __b));
int scandir64(
	const char* __path,
	struct dirent64*** __res,
	int (*__filter)(const struct dirent64* __dirent),
	int (*__compar)(const struct dirent64** __a, const struct dirent64** __b));
int alphasort(const struct dirent** __a, const struct dirent** __b);
int alphasort64(const struct dirent64** __a, const struct dirent64** __b);

// glibc
int versionsort(const struct dirent** __a, const struct dirent** __b);
int versionsort64(const struct dirent64** __a, const struct dirent64** __b);

__end_decls

#endif
