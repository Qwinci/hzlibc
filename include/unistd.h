#ifndef _UNISTD_H
#define _UNISTD_H

#include <bits/utils.h>
#include <bits/seek.h>
#include <sys/types.h>
#include <sys/select.h>
#include <termios.h>

__begin_decls

#ifndef _XOPEN_VERSION
#define _XOPEN_VERSION 700
#endif

#define _POSIX_VERSION 200809L
#define _POSIX_ADVISORY_INFO 200809L
#define _POSIX_ASYNCHRONOUS_IO 200809L
#define _POSIX_BARRIERS 200809L
#define _POSIX_CHOWN_RESTRICTED 1
#define _POSIX_CLOCK_SELECTION 200809L
#define _POSIX_CPUTIME 200809L
#define _POSIX_FSYNC 200809L
#define _POSIX_IPV6 200809L
#define _POSIX_JOB_CONTROL 1
#define _POSIX_MAPPED_FILES 200809L
#define _POSIX_MEMLOCK 200809L
#define _POSIX_MEMLOCK_RANGE 200809L
#define _POSIX_MEMORY_PROTECTION 200809L
#define _POSIX_MESSAGE_PASSING 200809L
#define _POSIX_MONOTONIC_CLOCK 200809L
#define _POSIX_NO_TRUNC 1
#define _POSIX_PRIORITIZED_IO 200809L
#define _POSIX_PRIORITY_SCHEDULING 200809L
#define _POSIX_RAW_SOCKETS 200809L
#define _POSIX_READER_WRITER_LOCKS 200809L
#define _POSIX_REALTIME_SIGNALS 200809L
#define _POSIX_REGEXP 1
#define _POSIX_SAVED_IDS 1
#define _POSIX_SEMAPHORES 200809L
#define _POSIX_SHARED_MEMORY_OBJECTS 200809L
#define _POSIX_SHELL 1
#define _POSIX_SPAWN 200809L
#define _POSIX_SPIN_LOCKS 200809L
#define _POSIX_SYNCHRONIZED_IO 200809L
#define _POSIX_THREAD_ATTR_STACKADDR 200809L
#define _POSIX_THREAD_ATTR_STACKSIZE 200809L
#define _POSIX_THREAD_CPUTIME 200809L
#define _POSIX_THREAD_PROCESS_SHARED 200809L
#define _POSIX_THREAD_SAFE_FUNCTIONS 200809L
#define _POSIX_THREADS 200809L
#define _POSIX_TIMEOUTS 200809L
#define _POSIX_TIMERS 200809L
#define _POSIX_VDISABLE 0
#define _XOPEN_SHM 1
#define _XOPEN_UNIX 1

#define _SC_ARG_MAX 0
#define _SC_CHILD_MAX 1
#define _SC_CLK_TCK 2
#define _SC_NGROUPS_MAX 3
#define _SC_OPEN_MAX 4
#define _SC_STREAM_MAX 5
#define _SC_TZNAME_MAX 6
#define _SC_JOB_CONTROL 7

#define _SC_SAVED_IDS 8
#define _SC_REALTIME_SIGNALS 9
#define _SC_PRIORITY_SCHEDULING 10
#define _SC_TIMERS 11
#define _SC_ASYNCHRONOUS_IO 12
#define _SC_PRIORITIZED_IO 13
#define _SC_SYNCHRONIZED_IO 14
#define _SC_FSYNC 15
#define _SC_MAPPED_FILES 16
#define _SC_MEMLOCK 17
#define _SC_MEMLOCK_RANGE 18
#define _SC_MEMORY_PROTECTION 19
#define _SC_MESSAGE_PASSING 20
#define _SC_SEMAPHORES 21
#define _SC_SHARED_MEMORY_OBJECTS 22
#define _SC_AIO_LISTIO_MAX 23
#define _SC_AIO_MAX 24
#define _SC_AIO_PRIO_DELTA_MAX 25
#define _SC_DELAYTIMER_MAX 26
#define _SC_MQ_OPEN_MAX 27
#define _SC_MQ_PRIO_MAX 28
#define _SC_VERSION 29
#define _SC_PAGE_SIZE 30
#define _SC_PAGESIZE _SC_PAGE_SIZE
#define _SC_RTSIG_MAX 31
#define _SC_SEM_NSEMS_MAX 32
#define _SC_SEM_VALUE_MAX 33
#define _SC_SIGQUEUE_MAX 34
#define _SC_TIMER_MAX 35
#define _SC_BC_BASE_MAX 36
#define _SC_BC_DIM_MAX 37
#define _SC_BC_SCALE_MAX 38
#define _SC_BC_STRING_MAX 39
#define _SC_COLL_WEIGHTS_MAX 40
#define _SC_EXPR_NEST_MAX 42
#define _SC_LINE_MAX 43
#define _SC_RE_DUP_MAX 44
#define _SC_2_VERSION 46
#define _SC_2_C_BIND 47
#define _SC_2_C_DEV 48
#define _SC_2_FORT_DEV 49
#define _SC_2_FORT_RUN 50
#define _SC_2_SW_DEV 51
#define _SC_2_LOCALEDEF 52
#define _SC_IOV_MAX 60
#define _SC_UIO_MAXIOV _SC_IOV_MAX
#define _SC_THREADS 67
#define _SC_THREAD_SAFE_FUNCTIONS 68
#define _SC_GETGR_R_SIZE_MAX 69
#define _SC_GETPW_R_SIZE_MAX 70
#define _SC_LOGIN_NAME_MAX 71
#define _SC_TTY_NAME_MAX 72
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 73
#define _SC_THREAD_KEYS_MAX 74
#define _SC_THREAD_STACK_MIN 75
#define _SC_THREAD_THREADS_MAX 76
#define _SC_THREAD_ATTR_STACKADDR 77
#define _SC_THREAD_ATTR_STACKSIZE 78
#define _SC_THREAD_PRIORITY_SCHEDULING 79
#define _SC_THREAD_PRIO_INHERIT 80
#define _SC_THREAD_PRIO_PROTECT 81
#define _SC_THREAD_PROCESS_SHARED 82
#define _SC_NPROCESSORS_CONF 83
#define _SC_NPROCESSORS_ONLN 84
#define _SC_PHYS_PAGES 85
#define _SC_AVPHYS_PAGES 86
#define _SC_ATEXIT_MAX 87
#define _SC_PASS_MAX 88
#define _SC_XOPEN_VERSION 89
#define _SC_XOPEN_XCU_VERSION 90
#define _SC_XOPEN_UNIX 91
#define _SC_XOPEN_CRYPT 92
#define _SC_XOPEN_ENH_I18N 93
#define _SC_XOPEN_SHM 94
#define _SC_2_CHAR_TERM 95
#define _SC_2_UPE 97
#define _SC_XOPEN_XPG2 98
#define _SC_XOPEN_XPG3 99
#define _SC_XOPEN_XPG4 100
#define _SC_NZERO 109
#define _SC_XBS5_ILP32_OFF32 125
#define _SC_XBS5_ILP32_OFFBIG 126
#define _SC_XBS5_LP64_OFF64 127
#define _SC_XBS5_LPBIG_OFFBIG 128
#define _SC_XOPEN_LEGACY 129
#define _SC_XOPEN_REALTIME 130
#define _SC_XOPEN_REALTIME_THREADS 131
#define _SC_ADVISORY_INFO 132
#define _SC_BARRIERS 133
#define _SC_CLOCK_SELECTION 137
#define _SC_CPUTIME 138
#define _SC_THREAD_CPUTIME 139
#define _SC_MONOTONIC_CLOCK 149
#define _SC_READER_WRITER_LOCKS 153
#define _SC_SPIN_LOCKS 154
#define _SC_REGEXP 155
#define _SC_SHELL 157
#define _SC_SPAWN 159
#define _SC_SPORADIC_SERVER 160
#define _SC_THREAD_SPORADIC_SERVER 161
#define _SC_TIMEOUTS 164
#define _SC_TYPED_MEMORY_OBJECTS 165
#define _SC_2_PBS 168
#define _SC_2_PBS_ACCOUNTING 169
#define _SC_2_PBS_LOCATE 170
#define _SC_2_PBS_MESSAGE 171
#define _SC_2_PBS_TRACK 172
#define _SC_SYMLOOP_MAX 173
#define _SC_STREAMS 174
#define _SC_2_PBS_CHECKPOINT 175
#define _SC_V6_ILP32_OFF32 176
#define _SC_V6_ILP32_OFFBIG 177
#define _SC_V6_LP64_OFF64 178
#define _SC_V6_LPBIG_OFFBIG 179
#define _SC_HOST_NAME_MAX 180
#define _SC_TRACE 181
#define _SC_TRACE_EVENT_FILTER 182
#define _SC_TRACE_INHERIT 183
#define _SC_TRACE_LOG 184

#define _PC_LINK_MAX 0
#define _PC_MAX_CANON 1
#define _PC_MAX_INPUT 2
#define _PC_NAME_MAX 3
#define _PC_PATH_MAX 4
#define _PC_PIPE_BUF 5
#define _PC_CHOWN_RESTRICTED 6
#define _PC_NO_TRUNC 7
#define _PC_VDISABLE 8
#define _PC_SYNC_IO 9
#define _PC_ASYNC_IO 10
#define _PC_PRIO_IO 11
#define _PC_SOCK_MAXBUF 12
#define _PC_FILESIZEBITS 13
#define _PC_REC_INCR_XFER_SIZE 14
#define _PC_REC_MAX_XFER_SIZE 15
#define _PC_REC_MIN_XFER_SIZE 16
#define _PC_REC_XFER_ALIGN 17
#define _PC_ALLOC_SIZE_MIN 18
#define _PC_SYMLINK_MAX 19
#define _PC_2_SYMLINKS 20

#define _CS_PATH 0

#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#define F_ULOCK 0
#define F_LOCK 1
#define F_TLOCK 2
#define F_TEST 3

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

__attribute__((noreturn)) void _exit(int __status);

ssize_t read(int __fd, void* __buf, size_t __count);
ssize_t write(int __fd, const void* __buf, size_t __count);
ssize_t pread(int __fd, void* __buf, size_t __count, off_t __offset);
ssize_t pread64(int __fd, void* __buf, size_t __count, off64_t __offset);
ssize_t pwrite(int __fd, const void* __buf, size_t __count, off_t __offset);
ssize_t pwrite64(int __fd, const void* __buf, size_t __count, off64_t __offset);
off_t lseek(int __fd, off_t __offset, int __whence);
off64_t lseek64(int __fd, off64_t __offset, int __whence);
int close(int __fd);
int pipe(int __pipe_fd[2]);
int dup(int __old_fd);
int dup2(int __old_fd, int __new_fd);
int lockf(int __fd, int __op, off_t __len);

int access(const char* __path, int __mode);
int faccessat(int __dir_fd, const char* __path, int __mode, int __flags);
ssize_t readlink(const char* __restrict __path, char* __restrict __buf, size_t __buf_size);
int readlinkat(int __dir_fd, const char* __restrict __path, char* __restrict __buf, size_t __buf_size);
int chown(const char* __path, uid_t __owner, gid_t __group);
int lchown(const char* __path, uid_t __owner, gid_t __group);
int fchown(int __fd, uid_t __owner, gid_t __group);
int fchownat(int __dir_fd, const char* __path, uid_t __owner, gid_t __group, int __flags);
int link(const char* __old_path, const char* __new_path);
int symlink(const char* __target, const char* __link_path);
int unlink(const char* __path);
int unlinkat(int __dir_fd, const char* __path, int __flags);
int truncate(const char* __path, off_t __length);
int truncate64(const char* __path, off64_t __length);
int ftruncate(int __fd, off_t __length);
int ftruncate64(int __fd, off64_t __length);
int rmdir(const char* __path);
int sync(void);
int fsync(int __fd);
int fdatasync(int __fd);

int pathconf(const char* __path, int __name);
int fpathconf(int __fd, int __name);

size_t confstr(int __name, char* __buffer, size_t __size);

int isatty(int __fd);
char* ttyname(int __fd);
int ttyname_r(int __fd, char* __buffer, size_t __size);

pid_t getpgid(pid_t __pid);
pid_t getpgrp(void);

char* getlogin(void);
int getlogin_r(char* __buf, size_t __buf_size);

pid_t tcgetpgrp(int __fd);
int tcsetpgrp(int __fd, pid_t __pgrp);
int tcgetattr(int __fd, struct termios* __termios);
int tcsetattr(int __fd, int __optional_actions, const struct termios* __termios);
int tcsendbreak(int __fd, int __duration);
int tcdrain(int __fd);
int tcflush(int __fd, int __queue_selector);
int tcflow(int __fd, int __action);
void cfmakeraw(struct termios* __termios);
speed_t cfgetispeed(const struct termios* __termios);
int cfsetispeed(struct termios* __termios, speed_t __speed);
speed_t cfgetospeed(const struct termios* __termios);
int cfsetospeed(struct termios* __termios, speed_t __speed);

int getentropy(void* __buffer, size_t __size);
long gethostid(void);

char* getcwd(char* __buf, size_t __size);
int gethostname(char* __name, size_t __len);
int chroot(const char* __path);
int chdir(const char* __path);
int fchdir(int __fd);
int setuid(uid_t __uid);
int setgid(gid_t __gid);
pid_t setsid(void);
int setpgid(pid_t __pid, pid_t __pgid);
uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
pid_t getsid(pid_t __pid);
int getpagesize(void);
long sysconf(int __name);

int execv(const char* __path, char* const __argv[]);
int execve(const char* __path, char* const __argv[], char* const __envp[]);
int execl(const char* __path, const char* __arg, ...);
int execle(const char* __path, const char* __arg, ...);
int execlp(const char* __file, const char* __arg, ...);
int execvp(const char* __file, char* const __argv[]);
pid_t fork(void);
pid_t vfork(void);
pid_t getpid(void);
pid_t getppid(void);

unsigned int sleep(unsigned int __seconds);
int usleep(useconds_t __usec);
unsigned int alarm(unsigned int __seconds);

int pause(void);

extern char** environ;

extern char* optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getopt(int __argc, char* const __argv[], const char* __opt_string);

int getgroups(int __size, gid_t* __list);

// linux
int pipe2(int __pipe_fd[2], int __flags);
int dup3(int __old_fd, int __new_fd, int __flags);
ssize_t copy_file_range(int __fd_in, off64_t* __off_in, int __fd_out, off64_t* __off_out, size_t __len, unsigned int __flags);
int close_range(unsigned int __first, unsigned int __last, int __flags);

int execvpe(const char* __file, char* const __argv[], char* const __envp[]);

int setresuid(uid_t __ruid, uid_t __euid, uid_t __suid);
int getresuid(uid_t* __ruid, uid_t* __euid, uid_t* __suid);
int setresgid(uid_t __rgid, uid_t __egid, uid_t __sgid);
int getresgid(gid_t* __rgid, gid_t* __egid, gid_t* __sgid);

// glibc
int euidaccess(const char* __path, mode_t __mode);
int eaccess(const char* __path, mode_t __mode);

// bsd
void closefrom(int __low_fd);

__end_decls

#endif
