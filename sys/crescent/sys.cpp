#include "ansi_sys.hpp"
#include "log.hpp"
#include "errno.h"
#include "sys/mman.h"
#include "crescent/syscall.h"
#include "crescent/syscalls.h"
#include "utils.hpp"
#include "string.h"
#include "fcntl.h"
#include "thread.hpp"
#include "stdio.h"

#define STUB_ENOSYS println("hzlibc: ", __func__, " is a not implemented and returns ENOSYS"); return ENOSYS

void sys_libc_log(hz::string_view str) {
	syscall(SYS_SYSLOG, str.data(), str.size());
}

[[noreturn]] void sys_exit(int status) {
	syscall(SYS_PROCESS_EXIT, status);
	__builtin_trap();
}

[[noreturn]] void sys_exit_thread() {
	syscall(SYS_THREAD_EXIT, 0);
	__builtin_trap();
}

int sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off64_t offset, void** ret) {
	// this code is only valid for ansi-only usage of mmap from rtld

	if (flags & MAP_FIXED) {
		return 0;
	}

	int crescent_prot = CRESCENT_PROT_READ | CRESCENT_PROT_WRITE;

	__ensure(flags == (MAP_PRIVATE | MAP_ANON));
	__ensure(fd == -1);
	__ensure(offset == 0);

	auto status = static_cast<int>(syscall(SYS_MAP, &addr, length, crescent_prot));
	if (status == ERR_NO_MEM) {
		return ENOMEM;
	}
	else {
		__ensure(status == 0);
		*ret = addr;
		return 0;
	}
}

int sys_munmap(void* addr, size_t length) {
	auto status = static_cast<int>(syscall(SYS_UNMAP, addr, length));
	if (status == ERR_INVALID_ARGUMENT) {
		return EINVAL;
	}
	else {
		__ensure(status == 0);
		return 0;
	}
}

int sys_mprotect(void* addr, size_t length, int prot) {
	int crescent_prot = 0;
	if (prot & PROT_READ) {
		crescent_prot |= CRESCENT_PROT_READ;
	}
	if (prot & PROT_WRITE) {
		crescent_prot |= CRESCENT_PROT_WRITE;
	}
	if (prot & PROT_EXEC) {
		crescent_prot |= CRESCENT_PROT_EXEC;
	}

	auto status = static_cast<int>(syscall(SYS_PROTECT, addr, length, crescent_prot));
	if (status == ERR_INVALID_ARGUMENT) {
		return EINVAL;
	}
	else {
		__ensure(status == 0);
		return 0;
	}
}

int sys_openat(int dir_fd, const char* path, int flags, mode_t mode, int* ret) {
	int crescent_flags = 0;

	int acc_mode = flags & O_ACCMODE;
	if (acc_mode == O_RDONLY) {
		crescent_flags = OPEN_READ;
	}
	else if (acc_mode == O_WRONLY) {
		crescent_flags = OPEN_WRITE;
	}
	else {
		crescent_flags = OPEN_READ_WRITE;
	}

	CrescentHandle ret_handle;

	CrescentHandle dir_handle;
	if (dir_fd == AT_FDCWD) {
		dir_handle = INVALID_CRESCENT_HANDLE;
	}
	else {
		dir_handle = static_cast<CrescentHandle>(dir_fd);
	}

	auto status = syscall(
		SYS_OPENAT,
		&ret_handle,
		dir_handle,
		path,
		strlen(path),
		crescent_flags);
	if (status == ERR_INVALID_ARGUMENT) {
		return EINVAL;
	}
	else if (status == ERR_NOT_EXISTS) {
		return ENOENT;
	}
	else {
		__ensure(status == 0);
		*ret = static_cast<int>(ret_handle);
		return 0;
	}
}

int sys_close(int fd) {
	auto status = static_cast<int>(syscall(SYS_CLOSE_HANDLE, static_cast<CrescentHandle>(fd)));
	if (status == ERR_INVALID_ARGUMENT) {
		return EBADF;
	}
	else {
		__ensure(status == 0);
		return 0;
	}
}

int sys_read(int fd, void* buf, size_t count, ssize_t* ret) {
	size_t actual;
	auto status = static_cast<int>(syscall(
		SYS_READ,
		static_cast<CrescentHandle>(fd),
		buf,
		count,
		&actual));
	if (status == ERR_INVALID_ARGUMENT) {
		return EBADF;
	}
	else {
		__ensure(status == 0);
		*ret = static_cast<ssize_t>(actual);
		return 0;
	}
}

int sys_write(int fd, const void* buf, size_t count, ssize_t* ret) {
	size_t actual;
	auto status = static_cast<int>(syscall(
		SYS_WRITE,
		static_cast<CrescentHandle>(fd),
		buf,
		count,
		&actual));
	if (status == ERR_INVALID_ARGUMENT) {
		return EBADF;
	}
	else {
		__ensure(status == 0);
		*ret = static_cast<ssize_t>(actual);
		return 0;
	}
}

int sys_lseek(int fd, off64_t offset, int whence, off64_t* ret) {
	uint64_t value = 0;
	auto status = static_cast<int>(syscall(
		SYS_SEEK,
		static_cast<CrescentHandle>(fd),
		static_cast<uint64_t>(offset),
		whence,
		&value));
	if (status == ERR_INVALID_ARGUMENT) {
		return EINVAL;
	}
	else {
		__ensure(status == 0);
		*ret = static_cast<off64_t>(value);
		return 0;
	}
}

int sys_remove(const char* path) {
	STUB_ENOSYS;
}

int sys_renameat(int old_dir_fd, const char* old_path, int new_dir_fd, const char* new_path) {
	STUB_ENOSYS;
}

int sys_stat(StatTarget target, int dir_fd, const char* path, int flags, struct stat64* s) {
	STUB_ENOSYS;
}

int sys_sleep(const timespec64* duration, timespec64* rem) {
	auto ns = duration->tv_sec * 1000 * 1000 * 1000 + duration->tv_nsec;
	auto us = ns / 1000;
	__ensure(syscall(SYS_SLEEP, us) == 0);
	if (rem) {
		rem->tv_sec = 0;
		rem->tv_nsec = 0;
	}
	return 0;
}

// only CLOCK_REALTIME is used by timespec_get
int sys_clock_gettime(clockid_t id, timespec* tp) {
	STUB_ENOSYS;
}

// only used with ansi only, this differs in that it should set errno
int sys_system(const char* cmd) {
	STUB_ENOSYS;
}

int sys_sigprocmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old) {
	STUB_ENOSYS;
}

int sys_sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old) {
	STUB_ENOSYS;
}

int sys_raise(int sig_num) {
	STUB_ENOSYS;
}

int sys_futex_wait(int* addr, int value, const timespec* timeout, bool pshared) {
	uint64_t timeout_ns = UINT64_MAX;
	if (timeout) {
		timeout_ns = timeout->tv_sec * 1000 * 1000 * 1000 + timeout->tv_nsec;
	}

	auto status = static_cast<int>(syscall(
		SYS_FUTEX_WAIT,
		addr,
		value,
		timeout_ns));
	if (status == ERR_TIMED_OUT) {
		return ETIMEDOUT;
	}
	else if (status == ERR_TRY_AGAIN) {
		return EAGAIN;
	}
	else {
		__ensure(status == 0);
		return 0;
	}
}

int sys_futex_wake(int* addr, bool pshared) {
	auto status = static_cast<int>(syscall(SYS_FUTEX_WAKE, addr, 1));
	__ensure(status == 0);
	return 0;
}

int sys_futex_wake_all(int* addr, bool pshared) {
	auto status = static_cast<int>(syscall(SYS_FUTEX_WAKE, addr, INT_MAX));
	__ensure(status == 0);
	return 0;
}

int sys_sched_yield() {
	__ensure(syscall(SYS_YIELD) == 0);
	return 0;
}

struct ThreadInfo {
	void* (*start_fn)(void* arg);
	void* arg;
	void* tp;
};

static void thread_entry(void* arg) {
	auto* info = static_cast<ThreadInfo*>(arg);
	auto fn = info->start_fn;
	auto thread_arg = info->arg;

	__ensure(sys_tcb_set(info->tp) == 0);
	delete info;

	hzlibc_thread_entry(fn, thread_arg);
}

int sys_thread_create(
	void* stack_base,
	size_t stack_size,
	void* (start_fn)(void* arg),
	void* arg,
	void* tp,
	pid_t* tid) {
	auto info = new ThreadInfo {
		.start_fn = start_fn,
		.arg = arg,
		.tp = tp
	};

	CrescentHandle handle;
	auto status = static_cast<int>(syscall(
		SYS_THREAD_CREATE,
		&handle,
		"libc thread",
		sizeof("libc thread") - 1,
		thread_entry,
		info));
	__ensure(status == 0);
	return 0;
}

int sys_tcb_set(void* tp) {
#ifdef __x86_64__
	__ensure(syscall(SYS_SET_FS_BASE, tp) == 0);
	return 0;
#else
#error missing architecture specific code
#endif
}

pid_t sys_get_thread_id() {
	return static_cast<pid_t>(syscall(SYS_GET_THREAD_ID));
}

int sys_getpagesize() {
	return 0x1000;
}
