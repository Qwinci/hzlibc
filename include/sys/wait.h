#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <bits/utils.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <signal.h>

__begin_decls

#define WNOHANG 1
#define WUNTRACED 2
#define WCONTINUED 8

#define WEXITSTATUS(status) (((status) >> 8) & 0xFF)
#define WTERMSIG(status) ((status) & 0x7F)
#define WSTOPSIG(status) WEXITSTATUS(status)
#define WIFEXITED(status) (WTERMSIG(status) == 0)
#define WIFSIGNALED(status) ((signed char) (((status) & 0x7F) + 1) >> 1) > 0)
#define WIFSTOPPED(status) (((status) & 0xFF) == 0x7F)
#define WIFCONTINUED(status) ((status) == __W_CONTINUED)

#define WCOREFLAG 0x80
#define WCOREDUMP(status) ((status) & WCOREFLAG)

#define W_EXITCODE(ret, sig) ((ret) << 8 | (sig))
#define W_STOPCODE(sig) ((sig) << 8 | 0x7F)
#define __W_CONTINUED 0xFFFF

typedef enum {
	P_ALL,
	P_PID,
	P_PGID,
	P_PIDFD
} idtype_t;

typedef uint32_t id_t;

pid_t wait(int* __status);
pid_t waitpid(pid_t __pid, int* __status, int __options);
int waitid(idtype_t __id_type, id_t __id, siginfo_t* __info, int __options);

// linux
pid_t wait4(pid_t __pid, int* __status, int __options, struct rusage* __rusage);

__end_decls

#endif
