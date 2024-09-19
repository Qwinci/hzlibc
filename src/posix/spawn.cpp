#include "spawn.h"
#include "utils.hpp"
#include "stdlib.h"
#include "errno.h"
#include "sys.hpp"
#include "fcntl.h"
#include "unistd.h"
#include <hz/new.hpp>

enum class Type {
	Open,
	Close,
	Dup2
};

struct __spawn_action {
	Type type;
	union {
		struct {
			int fd;
			const char* path;
			int oflag;
			mode_t mode;
		} open;

		struct {
			int fd;
		} close;

		struct {
			int old_fd;
			int new_fd;
		} dup2;
	};
};

EXPORT int posix_spawn_file_actions_init(posix_spawn_file_actions_t* file_actions) {
	*file_actions = {};
	return 0;
}

EXPORT int posix_spawn_file_actions_destroy(posix_spawn_file_actions_t* file_actions) {
	free(file_actions->__actions);
	return 0;
}

EXPORT int posix_spawn_file_actions_addopen(
	posix_spawn_file_actions_t* __restrict file_actions,
	int fd,
	const char* __restrict path,
	int oflag,
	mode_t mode) {
	if (file_actions->__used == file_actions->__allocated) {
		int new_size = file_actions->__allocated < 8 ?
			8 :
			file_actions->__allocated + file_actions->__allocated / 2;
		auto* ptr = realloc(file_actions->__actions, new_size * sizeof(__spawn_action));
		if (!ptr) {
			return ENOMEM;
		}
		file_actions->__actions = static_cast<__spawn_action*>(ptr);
	}

	auto action = &file_actions->__actions[file_actions->__used++];
	new (action) __spawn_action {
		.type = Type::Open,
		.open {
			.fd = fd,
			.path = path,
			.oflag = oflag,
			.mode = mode
		}
	};
	return 0;
}

EXPORT int posix_spawn_file_actions_addclose(posix_spawn_file_actions_t* file_actions, int fd) {
	if (file_actions->__used == file_actions->__allocated) {
		int new_size = file_actions->__allocated < 8 ?
		               8 :
		               file_actions->__allocated + file_actions->__allocated / 2;
		auto* ptr = realloc(file_actions->__actions, new_size * sizeof(__spawn_action));
		if (!ptr) {
			return ENOMEM;
		}
		file_actions->__actions = static_cast<__spawn_action*>(ptr);
	}

	auto action = &file_actions->__actions[file_actions->__used++];
	new (action) __spawn_action {
		.type = Type::Close,
		.close {
			.fd = fd
		}
	};
	return 0;
}

EXPORT int posix_spawn_file_actions_adddup2(posix_spawn_file_actions_t* file_actions, int fd, int new_fd) {
	if (file_actions->__used == file_actions->__allocated) {
		int new_size = file_actions->__allocated < 8 ?
		               8 :
		               file_actions->__allocated + file_actions->__allocated / 2;
		auto* ptr = realloc(file_actions->__actions, new_size * sizeof(__spawn_action));
		if (!ptr) {
			return ENOMEM;
		}
		file_actions->__actions = static_cast<__spawn_action*>(ptr);
	}

	auto action = &file_actions->__actions[file_actions->__used++];
	new (action) __spawn_action {
		.type = Type::Dup2,
		.dup2 {
			.old_fd = fd,
			.new_fd = new_fd
		}
	};
	return 0;
}

EXPORT int posix_spawnattr_init(posix_spawnattr_t* attr) {
	*attr = {};
	return 0;
}

EXPORT int posix_spawnattr_destroy(posix_spawnattr_t* attr) {
	return 0;
}

EXPORT int posix_spawnattr_setflags(posix_spawnattr_t* attr, short flags) {
	attr->__flags = flags;
	return 0;
}

EXPORT int posix_spawnattr_setpgroup(posix_spawnattr_t* attr, pid_t pgrp) {
	attr->__pgrp = pgrp;
	return 0;
}

EXPORT int posix_spawnattr_setschedpolicy(posix_spawnattr_t* attr, int sched_policy) {
	attr->__policy = sched_policy;
	return 0;
}

EXPORT int posix_spawnattr_setschedparam(posix_spawnattr_t* __restrict attr, const sched_param* __restrict param) {
	attr->__sp = *param;
	return 0;
}

EXPORT int posix_spawnattr_setsigmask(posix_spawnattr_t* __restrict attr, const sigset_t* __restrict sig_mask) {
	attr->__ss = *sig_mask;
	return 0;
}

EXPORT int posix_spawnattr_setsigdefault(posix_spawnattr_t* __restrict attr, const sigset_t* __restrict sig_default) {
	attr->__sd = *sig_default;
	return 0;
}

EXPORT int posix_spawn(
	pid_t* __restrict pid,
	const char* __restrict path,
	const posix_spawn_file_actions_t* __restrict file_actions,
	const posix_spawnattr_t* __restrict attr,
	char* const argv[],
	char* const envp[]) {
	if (attr && (attr->__flags & ~
		(POSIX_SPAWN_USEVFORK | POSIX_SPAWN_SETSIGDEF |
		POSIX_SPAWN_SETSIGMASK | POSIX_SPAWN_SETPGROUP |
		POSIX_SPAWN_SETSCHEDULER))) {
		panic("posix_spawn: unsupported flags ", Fmt::Hex, attr->__flags, Fmt::Dec);
	}

	posix_spawn_file_actions_t default_actions {};
	if (!file_actions) {
		file_actions = &default_actions;
	}

	pid_t new_pid;
	if (auto err = sys_fork(&new_pid)) {
		return err;
	}

	if (!new_pid) {
		if (attr) {
			if (attr->__flags & POSIX_SPAWN_SETSIGDEF) {
				for (int i = 1; i < NSIG; ++i) {
					if (sigismember(&attr->__sd, i)) {
						struct sigaction act {};
						act.sa_handler = SIG_DFL;
						if (sigaction(i, &act, nullptr) < 0) {
							exit(127);
						}
					}
				}
			}

			if (attr->__flags & POSIX_SPAWN_SETSIGMASK) {
				if (sigprocmask(SIG_SETMASK, &attr->__ss, nullptr) < 0) {
					exit(127);
				}
			}

			if (attr->__flags & POSIX_SPAWN_SETPGROUP) {
				if (setpgid(0, attr->__pgrp) < 0) {
					exit(127);
				}
			}

			if (attr->__flags & POSIX_SPAWN_SETSCHEDULER) {
				if (sys_sched_setscheduler(0, attr->__policy, &attr->__sp)) {
					exit(127);
				}
			}
		}

		for (int i = 0; i < file_actions->__used; i++) {
			auto& action = file_actions->__actions[i];

			switch (action.type) {
				case Type::Open:
				{
					close(action.open.fd);
					int fd = open(action.open.path, action.open.oflag, action.open.mode);
					if (fd < 0) {
						exit(127);
					}
					if (dup2(fd, action.open.fd) < 0) {
						exit(127);
					}
					break;
				}
				case Type::Close:
				{
					close(action.close.fd);
					break;
				}
				case Type::Dup2:
				{
					if (dup2(action.dup2.old_fd, action.dup2.new_fd) < 0) {
						exit(127);
					}
					break;
				}
			}
		}

		execve(path, argv, envp);
		exit(127);
	}

	*pid = new_pid;
	return 0;
}
