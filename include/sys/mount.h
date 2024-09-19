#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#include <bits/utils.h>

__begin_decls

#define MNT_FORCE 1
#define MNT_DETACH 2
#define MNT_EXPIRE 4
#define UMOUNT_NOFOLLOW 8

int mount(const char* __source, const char* __target, const char* __fs_type, unsigned long __flags, const void* __data);
int umount(const char* __target);
int umount2(const char* __target, int __flags);

__end_decls

#endif
