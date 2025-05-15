#ifndef _SYS_SEM_H
#define _SYS_SEM_H

#include <bits/utils.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>

__begin_decls

#define GETPID 11
#define GETVAL 12
#define GETALL 13
#define GETNCNT 14
#define GETZCNT 15
#define SETVAL 16
#define SETALL 17

#define SEM_STAT 18
#define SEM_INFO 19
#define SEM_STAT_ANY 20
#define SEM_UNDO 0x1000

#define _SEM_SEMUN_UNDEFINED 1

struct sembuf {
	unsigned short sem_num;
	short sem_op;
	short sem_flg;
};

struct semid_ds {
	struct ipc_perm sem_perm;
#if UINTPTR_MAX == UINT32_MAX
	time_t sem_otime;
	unsigned long __sem_otime_high;
	time_t sem_ctime;
	unsigned long __sem_ctime_high;
#else
	time_t sem_otime;
	time_t sem_ctime;
#endif
	unsigned long sem_nsems;
	unsigned long __unused[2];
};

int semget(key_t __key, int __num_sems, int __sem_flag);
int semop(int __sem_id, struct sembuf* __sops, size_t __num_sops);
int semctl(int __sem_id, int __sem_num, int __cmd, ...);

__end_decls

#endif
