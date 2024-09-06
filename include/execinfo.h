#ifndef _EXECINFO_H
#define _EXECINFO_H

#include <bits/utils.h>

__begin_decls

int backtrace(void** __buffer, int __size);
char** backtrace_symbols(const void** __buffer, int __size);

__end_decls

#endif
