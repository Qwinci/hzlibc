#pragma once
#include "time.h"
#include "bits/sigset_t.h"
#include "sys/types.h"

using time64_t = int64_t;

struct timespec64 {
	time64_t tv_sec;
	long tv_nsec;
};

void sys_libc_log(hz::string_view str);
[[noreturn]] void sys_exit(int status);
[[noreturn]] void sys_exit_thread();
int sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off64_t offset, void** ret);
int sys_munmap(void* addr, size_t length);
int sys_mprotect(void* addr, size_t length, int prot);

enum class StatTarget {
	Path,
	Fd,
	FdPath
};

int sys_openat(int dir_fd, const char* path, int flags, mode_t mode, int* ret);
int sys_close(int fd);
int sys_read(int fd, void* buf, size_t count, ssize_t* ret);
int sys_write(int fd, const void* buf, size_t count, ssize_t* ret);
int sys_lseek(int fd, off64_t offset, int whence, off64_t* ret);
int sys_remove(const char* path);
// only AT_FDCWD is used with ansi only
int sys_renameat(int old_dir_fd, const char* old_path, int new_dir_fd, const char* new_path);
int sys_stat(StatTarget target, int dir_fd, const char* path, int flags, struct stat64* s);

int sys_sleep(const timespec64* duration, timespec64* rem);

// only CLOCK_REALTIME and CLOCK_PROCESS_CPUTIME_ID are used with ansi only
int sys_clock_gettime(clockid_t id, timespec* tp);

// only used with ansi only, this differs in that it should set errno
int sys_system(const char* cmd);

int sys_sigprocmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old);
int sys_sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old);
int sys_raise(int sig_num);

int sys_futex_wait(int* addr, int value, const timespec* timeout, bool pshared = false);
int sys_futex_wake(int* addr, bool pshared = false);
int sys_futex_wake_all(int* addr, bool pshared = false);

int sys_sched_yield();
int sys_thread_create(
	void* stack_base,
	size_t stack_size,
	void* (start_fn)(void* arg),
	void* arg,
	void* tp,
	pid_t* tid);
int sys_tcb_set(void* tcb);
pid_t sys_get_thread_id();

int sys_getpagesize();
