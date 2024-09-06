#ifndef _LIBGEN_H
#define _LIBGEN_H

#include <bits/utils.h>

__begin_decls

extern char *__xpg_basename (char* __path);
#define basename __xpg_basename

__end_decls

#endif
