#include "spawn.h"
#include "utils.hpp"

EXPORT int posix_spawn_file_actions_init(posix_spawn_file_actions_t* file_actions) {
	__ensure(!"posix_spawn_file_actions_init is not implemented");
}

EXPORT int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t* file_actions) {
	__ensure(!"posix_spawn_file_actions_destroy is not implemented");
}

EXPORT int posix_spawn_file_actions_addopen(
	posix_spawn_file_actions_t* __restrict file_actions,
	int fd,
	const char* __restrict path,
	int oflag,
	mode_t mode) {
	__ensure(!"posix_spawn_file_actions_addopen is not implemented");
}

EXPORT int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t* file_actions, int fd, int new_fd) {
	__ensure(!"posix_spawn_file_actions_adddup2 is not implemented");
}

EXPORT int posix_spawn(
	pid_t* __restrict pid,
	const char* __restrict path,
	const posix_spawn_file_actions_t* __restrict file_actions,
	const posix_spawnattr_t* __restrict attr,
	char* const argv[],
	char* const envp[]) {
	__ensure(!"posix_spawn is not implemented");
}
