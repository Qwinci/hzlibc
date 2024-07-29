#ifndef _SYS_SHM_H
#define _SYS_SHM_H

#include <bits/utils.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <time.h>

__begin

typedef unsigned long shmatt_t;

struct shmid_ds {
	struct ipc_perm shm_perm;
	size_t shm_segsz;
#if UINTPTR_MAX == UINT32_MAX
	time_t shm_atime;
	unsigned long __shm_atime_high;
	time_t shm_dtime;
	unsigned long __shm_dtime_high;
	time_t shm_ctime;
	unsigned long __shm_ctime_high;
#else
	time_t shm_atime;
	time_t shm_dtime;
	time_t shm_ctime;
#endif
	pid_t shm_cpid;
	pid_t shm_lpid;
	shmatt_t shm_nattch;
	unsigned long __unused[2];
};

void* shmat(int __shm_id, const void* __shm_addr, int __shm_flag);
int shmdt(const void* __shm_addr);
int shmget(key_t __key, size_t __size, int __shm_flag);
int shmctl(int __shm_id, int __op, struct shmid_ds* __buf);

__end

#endif
