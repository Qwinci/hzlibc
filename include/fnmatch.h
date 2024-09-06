#ifndef _FNMATCH_H
#define _FNMATCH_H

#include <bits/utils.h>

__begin_decls

#define FNM_PATHNAME 1
#define FNM_NOESCAPE 2
#define FNM_PERIOD 4

#define FNM_NOMATCH 1

int fnmatch(const char* __pattern, const char* __str, int __flags);

// glibc
#define FNM_FILE_NAME FNM_PATHNAME
#define FNM_LEADING_DIR 8
#define FNM_CASEFOLD 16
#define FNM_EXTMATCH 32

__end_decls

#endif
