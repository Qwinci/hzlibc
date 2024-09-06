#ifndef _SYS_FILE_H
#define _SYS_FILE_H

#include <bits/utils.h>

__begin_decls

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

int flock(int __fd, int __operation);

__end_decls

#endif
