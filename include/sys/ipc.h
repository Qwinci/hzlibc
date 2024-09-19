#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

#define IPC_RMID 0
#define IPC_SET 1
#define IPC_STAT 2
#define IPC_INFO 3

#define IPC_CREAT 0x200
#define IPC_EXCL 0x400
#define IPC_NOWAIT 0x800

#define IPC_PRIVATE ((key_t) 0)

struct ipc_perm {
	key_t __key;
	uid_t uid;
	gid_t gid;
	uid_t cuid;
	gid_t cgid;
	mode_t mode;
	unsigned short __seq;
	unsigned short __pad;
	unsigned long __unused[2];
};

key_t ftok(const char* __path, int __proj_id);

__end_decls

#endif
