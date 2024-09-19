#ifndef _SYS_INOTIFY_H
#define _SYS_INOTIFY_H

#include <bits/utils.h>
#include <stdint.h>

__begin_decls

#define IN_ACCESS 1
#define IN_MODIFY 2
#define IN_ATTRIB 4
#define IN_CLOSE_WRITE 8
#define IN_CLOSE_NOWRITE 0x10
#define IN_CLOSE (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)
#define IN_OPEN 0x20
#define IN_MOVED_FROM 0x40
#define IN_MOVED_TO 0x80
#define IN_MOVE (IN_MOVED_FROM | IN_MOVED_TO)
#define IN_CREATE 0x100
#define IN_DELETE 0x200
#define IN_DELETE_SELF 0x400
#define IN_MOVE_SELF 0x800
#define IN_UNMOUNT 0x2000
#define IN_Q_OVERFLOW 0x4000
#define IN_IGNORED 0x8000
#define IN_CLOSE (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE)
#define IN_MOVE (IN_MOVED_FROM | IN_MOVED_TO)
#define IN_ONLYDIR 0x1000000
#define IN_DONT_FOLLOW 0x2000000
#define IN_EXCL_UNLINK 0x4000000
#define IN_MASK_CREATE 0x10000000
#define IN_MASK_ADD 0x20000000
#define IN_ISDIR 0x40000000
#define IN_ONESHOT 0x80000000

#define IN_NONBLOCK 0x800
#define IN_CLOEXEC 0x80000

struct inotify_event {
	int wd;
	uint32_t mask;
	uint32_t cookie;
	uint32_t len;
	char name[];
};

int inotify_init(void);
int inotify_init1(int __flags);
int inotify_add_watch(int __fd, const char* __path, uint32_t __mask);
int inotify_rm_watch(int fd, int wd);

__end_decls

#endif
