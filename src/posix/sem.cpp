#include "sys/sem.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int semget(key_t key, int num_sems, int sem_flag) {
	int ret;
	if (auto err = sys_semget(key, num_sems, sem_flag, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int semop(int sem_id, struct sembuf* sops, size_t num_sops) {
	if (auto err = sys_semop(sem_id, sops, num_sops)) {
		errno = err;
		return -1;
	}
	return 0;
}

union semun {
	int val;
	struct semid_ds* buf;
	unsigned short* array;
	struct seminfo* __buf;
};

EXPORT int semctl(int sem_id, int sem_num, int cmd, ...) {
	semun arg {};
	switch (cmd) {
		case GETALL:
		case SETVAL:
		case SETALL:
		case IPC_SET:
		case IPC_STAT:
		case IPC_INFO:
		case SEM_STAT:
		case SEM_INFO:
		case SEM_STAT_ANY:
		{
			va_list ap;
			va_start(ap, cmd);
			arg = va_arg(ap, semun);
			va_end(ap);
			break;
		}
		default:
			break;
	}

	int ret;
	if (auto err = sys_semctl(sem_id, sem_num, cmd, arg.array, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
