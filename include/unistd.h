#ifndef _UNISTD_H
#define _UNISTD_H

#include <bits/utils.h>
#include <sys/types.h>

__begin

#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

__attribute__((noreturn)) void _exit(int __status);

ssize_t read(int __fd, void* __buf, size_t __count);
ssize_t write(int __fd, const void* __buf, size_t __count);
off_t lseek(int __fd, off_t __offset, int __whence);
int close(int __fd);
int pipe(int __pipe_fd[2]);

int access(const char* __path, int __mode);
int faccessat(int __dir_fd, const char* __path, int __mode, int __flags);
int readlink(const char* __restrict __path, char* __restrict __buf, size_t __buf_size);
int chown(const char* __path, uid_t __owner, gid_t __group);
int fchown(int __fd, uid_t __owner, gid_t __group);
int fchownat(int __dir_fd, const char* __path, uid_t __owner, gid_t __group, int __flags);

int isatty(int __fd);
pid_t tcgetpgrp(int __fd);
int getentropy(void* __buffer, size_t __size);

char* getcwd(char* __buf, size_t __size);
int gethostname(char* __name, size_t __len);
int chroot(const char* __path);
int chdir(const char* __path);
int fchdir(int __fd);
int setuid(uid_t __uid);
int setgid(gid_t __gid);
int getpagesize(void);

int execve(const char* __path, char* const __argv[], char* const __envp[]);
pid_t fork(void);

extern char** environ;

extern char* optarg;
extern int optind;
extern int opterr;
extern int optopt;

// bsd
int setgroups(size_t __size, const gid_t* __list);

// linux
int pipe2(int __pipe_fd[2], int __flags);
ssize_t copy_file_range(int __fd_in, off64_t* __off_in, int __fd_out, off64_t* __off_out, size_t __len, unsigned int __flags);

int execvpe(const char* __file, char* const __argv[], char* const __envp[]);

__end

#endif
