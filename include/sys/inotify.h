#ifndef _SYS_INOTIFY_H
#define _SYS_INOTIFY_H

#include <bits/utils.h>
#include <stdint.h>

__begin

int inotify_init(void);
int inotify_init1(int __flags);
int inotify_add_watch(int __fd, const char* __path, uint32_t __mask);
int inotify_rm_watch(int fd, int wd);

__end

#endif
