#include "ansi_sys.hpp"
#include "log.hpp"
#include "errno.h"
#include "sys/mman.h"
#include "sys/stat.h"
#include "crescent/syscall.h"
#include "crescent/syscalls.h"
#include "crescent/posix_syscall.h"
#include "crescent/posix_syscalls.h"
#include "utils.hpp"
#include "string.h"
#include "fcntl.h"
#include "thread.hpp"
#include "stdio.h"
#include "signal.h"

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

#define check_err(res) if ((res).err) return static_cast<int>((res).err)
#define get_err(res) static_cast<int>((res).err)

int sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off64_t offset, void** ret) {
	auto res = posix_syscall(SYS_POSIX_MMAP, addr, length, prot, flags, fd, offset);
	check_err(res);
	*ret = reinterpret_cast<void*>(res.ret);
	return 0;
}

int sys_munmap(void* addr, size_t length) {
	return get_err(posix_syscall(SYS_POSIX_MUNMAP, addr, length));
}

int sys_mprotect(void* addr, size_t length, int prot) {
	return get_err(posix_syscall(SYS_POSIX_MPROTECT, addr, length, prot));
}

int sys_openat(int dir_fd, const char* path, int flags, mode_t mode, int* ret) {
	int crescent_flags = 0;

	int acc_mode = flags & O_ACCMODE;

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
		if (status != 0) {
			auto ad = __builtin_return_address(0);
			auto addr = __builtin_extract_return_addr(ad);
			println(Fmt::Hex, reinterpret_cast<uintptr_t>(addr), Fmt::Dec);
		}
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
	switch (target) {
		case StatTarget::Path:
		{
			int fd;
			int status = sys_openat(AT_FDCWD, path, O_RDONLY, 0, &fd);
			if (status != 0) {
				return ENOENT;
			}

			CrescentStat crescent_stat {};
			status = syscall(SYS_STAT, static_cast<CrescentHandle>(fd), &crescent_stat);

			sys_close(fd);

			if (status == 0) {
				s->st_size = crescent_stat.size;
			}

			return status;
		}
		case StatTarget::Fd:
			break;
		case StatTarget::FdPath:
			break;
	}

	STUB_ENOSYS;
}

int sys_sleep(const timespec64* duration, timespec64* rem) {
	auto ns = duration->tv_sec * 1000 * 1000 * 1000 + duration->tv_nsec;
	__ensure(syscall(SYS_SLEEP, ns) == 0);
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
	return get_err(posix_syscall(SYS_POSIX_SIGPROCMASK, how, set, old));
}

extern "C" void __hzlibc_restorer();

#ifdef __x86_64__

asm(R"(
.pushsection .text

.globl __hzlibc_restorer
.hidden __hzlibc_restorer
__hzlibc_restorer:
	mov $4101, %edi
	syscall
	ud2
.popsection
)");

#elif defined(__aarch64__)

asm(R"(
.pushsection .text

.globl __hzlibc_restorer
.hidden __hzlibc_restorer
__hzlibc_restorer:
	mov x0, #4101
	svc #0
	udf #0
.popsection
)");

#else
#error missing architecture specific code
#endif

static_assert(SYS_POSIX_SIGRESTORE == 4101);

struct kernel_sigaction {
	void (*sa_handler)(int __sig_num);
	uint64_t sa_mask;
	unsigned long sa_flags;
	void (*sa_restorer)();
};

int sys_sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old) {
	kernel_sigaction act {};
	if (action) {
		act.sa_handler = action->sa_handler;
		memcpy(&act.sa_mask, &action->sa_mask, 8);
		act.sa_flags = action->sa_flags;
		act.sa_restorer = action->sa_restorer;
		if (!(act.sa_flags & SA_RESTORER)) {
			act.sa_restorer = __hzlibc_restorer;
			act.sa_flags |= SA_RESTORER;
		}
	}

	kernel_sigaction old_act {};

	auto err = get_err(posix_syscall(
		SYS_POSIX_SIGACTION,
		sig_num,
		action ? &act : nullptr,
		old ? &old_act : nullptr));
	if (old) {
		old->sa_handler = old_act.sa_handler;
		memcpy(&old->sa_mask, &old_act.sa_mask, 8);
		old->sa_flags = old_act.sa_flags;
		old->sa_restorer = old_act.sa_restorer;
	}

	return err;
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
	if (status == ERR_TIMEOUT) {
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
		info,
		tid));
	__ensure(status == 0);
	return 0;
}

int sys_tcb_set(void* tp) {
#ifdef __x86_64__
	__ensure(syscall(SYS_SET_FS_BASE, tp) == 0);
	return 0;
#elif defined(__aarch64__)
	asm volatile("msr tpidr_el0, %0" : : "r"(tp));
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
