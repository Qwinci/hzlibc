#ifndef _SPAWN_H
#define _SPAWN_H

#include <bits/utils.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

__begin

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
int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t* __file_actions, int __fd, int __new_fd);

int posix_spawn(
	pid_t* __restrict __pid,
	const char* __restrict __path,
	const posix_spawn_file_actions_t* __restrict __file_actions,
	const posix_spawnattr_t* __restrict __attr,
	char* const __argv[],
	char* const __envp[]);

__end

#endif
