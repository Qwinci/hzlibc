#ifndef _GLOB_H
#define _GLOB_H

#include <bits/utils.h>
#include <stddef.h>

__begin_decls

#define GLOB_ERR (1 << 0)
#define GLOB_MARK (1 << 1)
#define GLOB_NOSORT (1 << 2)
#define GLOB_DOOFFS (1 << 3)
#define GLOB_NOCHECK (1 << 4)
#define GLOB_APPEND (1 << 5)
#define GLOB_NOESCAPE (1 << 6)
#define GLOB_PERIOD (1 << 7)
#define GLOB_MAGCHAR (1 << 8)
#define GLOB_ALTDIRFUNC (1 << 9)
#define GLOB_BRACE (1 << 10)
#define GLOB_NOMAGIC (1 << 11)
#define GLOB_TILDE (1 << 12)
#define GLOB_ONLYDIR (1 << 13)
#define GLOB_TILDE_CHECK (1 << 14)

#define GLOB_NOSPACE 1
#define GLOB_ABORTED 2
#define GLOB_NOMATCH 3
#define GLOB_NOSYS 4

#ifdef __USE_GNU
struct dirent;
struct stat;
#endif

typedef struct {
	size_t gl_pathc;
	char** gl_pathv;
	size_t gl_offs;
	int gl_flags;
	void (*gl_closedir)(void* __arg);

#ifdef __USE_GNU
	struct dirent* (*gl_readdir)(void* __arg);
#else
	void* (*gl_readdir)(void* __arg);
#endif
	void* (*gl_opendir)(const char* __name);

#ifdef __USE_GNU
	int (*gl_lstat)(const char* __restrict __name, struct stat* __restrict __s);
	int (*gl_stat)(const char* __restrict __name, struct stat* __restrict __s);
#else
	int (*gl_lstat)(const char* __restrict __name, void* __restrict);
	int (*gl_stat)(const char* __restrict __name, void* __restrict);
#endif

} glob_t;

int glob(
	const char* __restrict __pattern,
	int __flags,
	int (*__err_func)(const char* __pat, int __err_num),
	glob_t* __restrict __glob);
void globfree(glob_t* __restrict __glob);

__end_decls

#endif
