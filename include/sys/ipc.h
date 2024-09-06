#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <bits/utils.h>

__begin_decls

#include <sys/types.h>

#define IPC_RMID 0
#define IPC_SET 1
#define IPC_STAT 2
#define IPC_INFO 3

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

__end_decls

#endif
