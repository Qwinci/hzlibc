#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

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

pid_t waitpid(pid_t __pid, int* __status, int __options);

__end

#endif
