#ifndef _NL_TYPES_H
#define _NL_TYPES_H

#include <bits/utils.h>

__begin_decls

#define NL_CAT_LOCALE 1

typedef void* nl_catd;
typedef int nl_item;

nl_catd catopen(const char* __cat_name, int __flag);
char* catgets(nl_catd __catalog, int __set, int __number, const char* __str);
int catclose(nl_catd __catalog);

__end_decls

#endif
