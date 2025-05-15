#include "sys/shm.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT int __getpagesize() {
	return sys_getpagesize();
}

EXPORT void* shmat(int shm_id, const void* shm_addr, int shm_flag) {
	void* ret;
	if (auto err = sys_shmat(shm_id, shm_addr, shm_flag, &ret)) {
		errno = err;
		return reinterpret_cast<void*>(-1);
	}
	return ret;
}

EXPORT int shmdt(const void* shm_addr) {
	if (auto err = sys_shmdt(shm_addr)) {
		errno = err;
		return -1;
	}
	return 0;
}

EXPORT int shmget(key_t key, size_t size, int shm_flag) {
	int ret;
	if (auto err = sys_shmget(key, size, shm_flag, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}

EXPORT int shmctl(int shm_id, int op, struct shmid_ds* buf) {
	int ret;
	if (auto err = sys_shmctl(shm_id, op, buf, &ret)) {
		errno = err;
		return -1;
	}
	return ret;
}
