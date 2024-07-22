#pragma once
#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/capability.h>
#include <sys/resource.h>
#include <sys/statfs.h>
#include <time.h>
#include <signal.h>
#include <hz/string_view.hpp>

void sys_libc_log(hz::string_view str);
[[noreturn]] void sys_exit(int status);
[[noreturn]] void sys_exit_thread();
int sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset, void** ret);
int sys_mprotect(void* addr, size_t length, int prot);
int sys_munmap(void* addr, size_t length);

int sys_openat(int dir_fd, const char* path, int flags, mode_t mode, int* ret);
int sys_close(int fd);
int sys_read(int fd, void* buf, size_t count, ssize_t* ret);
int sys_write(int fd, const void* buf, size_t count, ssize_t* ret);
int sys_lseek(int fd, off64_t offset, int whence, off64_t* ret);
int sys_ioctl(int fd, unsigned long op, void* arg, int* ret);
int sys_fcntl(int fd, int cmd, va_list args, int* ret);
int sys_fadvise(int fd, off64_t offset, off64_t len, int advice);
int sys_isatty(int fd);
int sys_pipe2(int pipe_fd[2], int flags);
int sys_copy_file_range(
	int fd_in,
	off64_t* off_in,
	int fd_out,
	off64_t* off_out,
	size_t len,
	unsigned int flags,
	ssize_t* ret);

enum class StatTarget {
	Path,
	Fd,
	FdPath
};

int sys_stat(StatTarget target, int dir_fd, const char* path, int flags, struct stat64* s);
int sys_statx(int dir_fd, const char* __restrict path, int flags, unsigned int mask, struct statx* __restrict buf);
int sys_statfs(const char* path, struct statfs64* buf);
int sys_fstatfs(int fd, struct statfs64* buf);

int sys_faccessat(int dir_fd, const char* path, int mode, int flags);
int sys_readlinkat(int dir_fd, const char* path, char* buf, size_t buf_size);
int sys_fchownat(int dir_fd, const char* path, uid_t owner, gid_t group, int flags);
int sys_open_dir(const char* path, int* handle);
int sys_read_dir(int handle, void* buffer, size_t max_size, size_t* count_read);

int sys_getxattr(const char* path, const char* name, void* value, size_t size, ssize_t* ret);
int sys_fgetxattr(int fd, const char* name, void* value, size_t size, ssize_t* ret);
int sys_setxattr(const char* path, const char* name, const void* value, size_t size, int flags);
int sys_fsetxattr(int fd, const char* name, const void* value, size_t size, int flags);
int sys_removexattr(const char* path, const char* name);
int sys_fremovexattr(int fd, const char* name);
int sys_listxattr(const char* path, char* list, size_t size, ssize_t* ret);

int sys_getcwd(char* buf, size_t size);
int sys_gethostname(char* name, size_t len);
int sys_uname(utsname* buf);
int sys_chroot(const char* path);
int sys_chdir(const char* path);
int sys_fchdir(int fd);
int sys_getpagesize();

int sys_tcb_set(void* tcb);
int sys_capget(cap_user_header_t hdr, cap_user_data_t data);
int sys_capset(cap_user_header_t hdr, cap_user_data_t data);
int sys_setgroups(size_t size, const gid_t* list);
int sys_getrandom(void* buffer, size_t size, unsigned int flags);

int sys_execve(const char* path, char* const argv[], char* const envp[]);
int sys_sched_yield();
int sys_prctl(int option, unsigned long arg0, unsigned long arg1, unsigned long arg2, unsigned long arg3);
int sys_setuid(uid_t uid);
int sys_setgid(gid_t gid);
int sys_waitpid(pid_t pid, int* status, int options, rusage* usage, pid_t* ret);
int sys_fork(pid_t* ret);

int sys_futex_wait(int* addr, int value, const timespec* timeout);
int sys_futex_wake(int* addr);
int sys_get_thread_id();
int sys_get_process_id();

int sys_sigprocmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old);
int sys_sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old);
int sys_kill(pid_t pid, int sig);

int sys_clock_gettime(clockid_t id, timespec* tp);

int sys_thread_create(
	void* stack_base,
	size_t stack_size,
	void* (start_fn)(void* arg),
	void* arg,
	void* tp,
	pid_t* tid);
