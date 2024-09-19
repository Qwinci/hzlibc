#ifndef _SYS_SWAP_H
#define _SYS_SWAP_H

#include <bits/utils.h>

__begin_decls

int swapon(const char* __path, int __flags);
int swapoff(const char* __path);

__end_decls

#endif
