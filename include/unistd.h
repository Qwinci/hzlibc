#ifndef _UNISTD_H
#define _UNISTD_H

#include <bits/utils.h>
#include <sys/types.h>
#include <termios.h>

__begin

#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

#define _SC_PAGESIZE 30
#define _SC_LOGIN_NAME_MAX 71
#define _SC_NPROCESSORS_CONF 83
#define _SC_PHYS_PAGES 85

__attribute__((noreturn)) void _exit(int __status);

ssize_t read(int __fd, void* __buf, size_t __count);
ssize_t write(int __fd, const void* __buf, size_t __count);
ssize_t pread(int __fd, void* __buf, size_t __count, off_t __offset);
ssize_t pwrite(int __fd, const void* __buf, size_t __count, off_t __offset);
off_t lseek(int __fd, off_t __offset, int __whence);
off64_t lseek64(int __fd, off64_t __offset, int __whence);
int close(int __fd);
int pipe(int __pipe_fd[2]);
int dup(int __old_fd);
int dup2(int __old_fd, int __new_fd);

int access(const char* __path, int __mode);
int faccessat(int __dir_fd, const char* __path, int __mode, int __flags);
int readlink(const char* __restrict __path, char* __restrict __buf, size_t __buf_size);
int chown(const char* __path, uid_t __owner, gid_t __group);
int fchown(int __fd, uid_t __owner, gid_t __group);
int fchownat(int __dir_fd, const char* __path, uid_t __owner, gid_t __group, int __flags);
int link(const char* __old_path, const char* __new_path);
int symlink(const char* __target, const char* __link_path);
int unlink(const char* __path);
int unlinkat(int __dir_fd, const char* __path, int __flags);
int truncate(const char* __path, off_t __length);
int ftruncate(int __fd, off_t __length);
int ftruncate64(int __fd, off64_t __length);
int rmdir(const char* __path);
int sync(void);
int fsync(int __fd);

int pathconf(const char* __path, int __name);
int fpathconf(int __fd, int __name);

int isatty(int __fd);

pid_t tcgetpgrp(int __fd);
int tcgetattr(int __fd, struct termios* __termios);
int tcsetattr(int __fd, int __optional_actions, const struct termios* __termios);
int tcflush(int __fd, int __queue_selector);
speed_t cfgetospeed(const struct termios* __termios);

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
int execlp(const char* __file, const char* __arg, ...);
int execvp(const char* __file, char* const __argv[]);
pid_t fork(void);
pid_t vfork(void);
pid_t getpid(void);

unsigned int sleep(unsigned int __seconds);
int usleep(useconds_t __usec);
unsigned int alarm(unsigned int __seconds);

extern char** environ;

extern char* optarg;
extern int optind;
extern int opterr;
extern int optopt;

int getgroups(size_t __size, gid_t* __list);

// linux
int pipe2(int __pipe_fd[2], int __flags);
ssize_t copy_file_range(int __fd_in, off64_t* __off_in, int __fd_out, off64_t* __off_out, size_t __len, unsigned int __flags);
int close_range(unsigned int __first, unsigned int __last, int __flags);

int execvpe(const char* __file, char* const __argv[], char* const __envp[]);

int getresuid(uid_t* __ruid, uid_t* __euid, uid_t* __suid);
int getresgid(gid_t* __rgid, gid_t* __egid, gid_t* __sgid);

// glibc
int euidaccess(const char* __path, mode_t __mode);
int eaccess(const char* __path, mode_t __mode);

__end

#endif
