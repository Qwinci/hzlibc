#ifndef _SPAWN_H
#define _SPAWN_H

#include <bits/utils.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

__begin_decls

typedef struct {
	short __flags;
	pid_t __pgrp;
	sigset_t __sd;
	sigset_t __ss;
	struct sched_param __sp;
	int __policy;
	int __cgroup;
	int __unused[15];
} posix_spawnattr_t;

#define POSIX_SPAWN_RESETIDS 1
#define POSIX_SPAWN_SETPGROUP 2
#define POSIX_SPAWN_SETSIGDEF 4
#define POSIX_SPAWN_SETSIGMASK 8
#define POSIX_SPAWN_SETSCHEDPARAM 0x10
#define POSIX_SPAWN_SETSCHEDULER 0x20
#define POSIX_SPAWN_USEVFORK 0x40
#define POSIX_SPAWN_SETSID 0x80
#define POSIX_SPAWN_SETCGROUP 0x100

typedef struct {
	int __allocated;
	int __used;
	struct __spawn_action* __actions;
	int __unused[16];
} posix_spawn_file_actions_t;

int posix_spawn_file_actions_init(posix_spawn_file_actions_t* __file_actions);
int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t* __file_actions);

int posix_spawn_file_actions_addopen(
	posix_spawn_file_actions_t* __restrict __file_actions,
	int __fd,
	const char* __restrict __path,
	int __oflag,
	mode_t __mode);
int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t* __file_actions, int __fd);
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t* __file_actions, int __fd, int __new_fd);

int posix_spawnattr_init(posix_spawnattr_t* __attr);
int posix_spawnattr_destroy(posix_spawnattr_t* __attr);
int posix_spawnattr_setflags(posix_spawnattr_t* __attr, short __flags);
int posix_spawnattr_setpgroup(posix_spawnattr_t* __attr, pid_t __pgrp);
int posix_spawnattr_setschedpolicy(posix_spawnattr_t* __attr, int __sched_policy);
int posix_spawnattr_setschedparam(posix_spawnattr_t* __restrict __attr, const struct sched_param* __restrict __param);
int posix_spawnattr_setsigmask(posix_spawnattr_t* __restrict __attr, const sigset_t* __restrict __sig_mask);
int posix_spawnattr_setsigdefault(posix_spawnattr_t* __restrict __attr, const sigset_t* __restrict __sig_default);

int posix_spawn(
	pid_t* __restrict __pid,
	const char* __restrict __path,
	const posix_spawn_file_actions_t* __restrict __file_actions,
	const posix_spawnattr_t* __restrict __attr,
	char* const __argv[],
	char* const __envp[]);

__end_decls

#endif
