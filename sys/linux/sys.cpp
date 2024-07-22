#include "sys.hpp"
#include "syscall.hpp"
#include "syscall.h"
#include "utils.hpp"
#include "fcntl.h"
#include "sys/ioctl.h"
#include "sys/mman.h"
#include "errno.h"
#include "string.h"
#include "limits.h"
#include "sched.h"

#define memcpy __builtin_memcpy

#if defined(__x86_64__) || defined(__i386__) || defined(__aarch64__)
#define _NSIG 65
#else
#error missing architecture specific code
#endif

void sys_libc_log(hz::string_view str) {
	syscall(SYS_write, 2, str.data(), str.size());
}

[[noreturn]] void sys_exit(int status) {
	syscall(SYS_exit_group, status);
	__builtin_trap();
}

[[noreturn]] void sys_exit_thread() {
	syscall(SYS_exit, 0);
	__builtin_trap();
}

int sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset, void** ret) {
#ifdef SYS_mmap2
	auto res = syscall(SYS_mmap2, addr, length, prot, flags, fd, offset / 0x1000);
#else
	auto res = syscall(SYS_mmap, addr, length, prot, flags, fd, offset);
#endif
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = reinterpret_cast<void*>(res);
	return 0;
}

int sys_mprotect(void* addr, size_t length, int prot) {
	return syscall_error(syscall(SYS_mprotect, addr, length, prot));
}

int sys_munmap(void* addr, size_t length) {
	return syscall_error(syscall(SYS_munmap, addr, length));
}

int sys_openat(int dir_fd, const char* path, int flags, mode_t mode, int* ret) {
	auto res = syscall(SYS_openat, dir_fd, path, flags, mode);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<int>(res);
	return 0;
}

int sys_close(int fd) {
	return syscall_error(syscall(SYS_close, fd));
}

int sys_read(int fd, void* buf, size_t count, ssize_t* ret) {
	auto res = syscall(SYS_read, fd, buf, count);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = res;
	return 0;
}

int sys_write(int fd, const void* buf, size_t count, ssize_t* ret) {
	auto res = syscall(SYS_write, fd, buf, count);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = res;
	return 0;
}

int sys_lseek(int fd, off64_t offset, int whence, off64_t* ret) {
#ifdef __i386__
	auto low = static_cast<uint32_t>(offset);
	auto high = static_cast<uint64_t>(offset) >> 32;
	auto res = syscall(SYS__llseek, fd, high, low, ret, whence);
#else
	auto res = syscall(SYS_lseek, fd, offset, whence);
#endif
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = res;
	return 0;
}

int sys_ioctl(int fd, unsigned long op, void* arg, int* ret) {
	auto res = syscall(SYS_ioctl, fd, op, arg);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<int>(res);
	return 0;
}

#ifndef SYS_fcntl64
#define SYS_fcntl64 SYS_fcntl
#endif

struct f_owner_ex {
	enum {
		F_OWNER_TID,
		F_OWNER_PID,
		F_OWNER_PGRP
	} type;
	pid_t pid;
};

int sys_fcntl(int fd, int cmd, va_list args, int* ret) {
	void* arg = va_arg(args, void*);

	switch (cmd) {
		case F_SETLK:
		case F_SETLKW:
		{
			auto* lock = static_cast<struct flock*>(arg);
			struct flock64 wide {
				.l_type = lock->l_type,
				.l_whence = lock->l_whence,
				.l_start = lock->l_start,
				.l_len = lock->l_len,
				.l_pid = lock->l_pid
			};
			auto res = syscall(SYS_fcntl64, fd, cmd, &wide);
			if (int err = syscall_error(res)) {
				return err;
			}
			*ret = static_cast<int>(res);
			return 0;
		}
		case F_GETLK:
		{
			auto* lock = static_cast<struct flock*>(arg);
			struct flock64 wide {
				.l_type = lock->l_type,
				.l_whence = lock->l_whence,
				.l_start = lock->l_start,
				.l_len = lock->l_len,
				.l_pid = lock->l_pid
			};

			auto res = syscall(SYS_fcntl64, fd, cmd, &wide);
			if (int err = syscall_error(res)) {
				return err;
			}
			if (wide.l_start > LONG_MAX ||
				wide.l_len > LONG_MAX) {
				return EOVERFLOW;
			}

			lock->l_type = wide.l_type;
			lock->l_whence = wide.l_whence;
			lock->l_start = static_cast<off_t>(wide.l_start);
			lock->l_len = static_cast<off_t>(wide.l_len);
			lock->l_pid = wide.l_pid;
			*ret = static_cast<int>(res);
			return 0;
		}
		case F_GETOWN:
		{
			f_owner_ex ex {};
			int err = syscall_error(syscall(SYS_fcntl64, fd, F_GETOWN_EX, &ex));
			if (err) {
				return err;
			}
			if (ex.type == f_owner_ex::F_OWNER_PGRP) {
				*ret = -ex.pid;
			}
			else {
				*ret = ex.pid;
			}
			return 0;
		}
		default:
		{
			auto res = syscall(SYS_fcntl64, fd, cmd, arg);
			if (int err = syscall_error(res)) {
				return err;
			}
			*ret = static_cast<int>(res);
			return 0;
		}
	}
}

int sys_fadvise(int fd, off64_t offset, off64_t len, int advice) {
#ifdef SYS_fadvise64_64
	return syscall_error(syscall(SYS_fadvise64_64, fd, offset, len, advice));
#else
	return syscall_error(syscall(SYS_fadvise64, fd, offset, len, advice));
#endif
}

int sys_isatty(int fd) {
	winsize size {};
	int tmp;
	if (sys_ioctl(fd, TIOCGWINSZ, &size, &tmp) != 0) {
		return ENOTTY;
	}
	return 0;
}

int sys_pipe2(int pipe_fd[2], int flags) {
	return syscall_error(syscall(SYS_pipe2, pipe_fd, flags));
}

int sys_copy_file_range(
	int fd_in,
	off64_t* off_in,
	int fd_out,
	off64_t* off_out,
	size_t len,
	unsigned int flags,
	ssize_t* ret) {
	auto res = syscall(SYS_copy_file_range, fd_in, off_in, fd_out, off_out, len, flags);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<ssize_t>(res);
	return 0;
}

int sys_stat(StatTarget target, int dir_fd, const char* path, int flags, struct stat64* s) {
	if (target == StatTarget::Path) {
		dir_fd = AT_FDCWD;
	}
	else if (target == StatTarget::Fd) {
		flags |= AT_EMPTY_PATH;
	}

#ifdef SYS_newfstatat
	return syscall_error(syscall(SYS_newfstatat, dir_fd, path, s, flags));
#else
	return syscall_error(syscall(SYS_fstatat64, dir_fd, path, s, flags));
#endif
}

int sys_statx(int dir_fd, const char* __restrict path, int flags, unsigned int mask, struct statx* __restrict buf) {
	return syscall_error(syscall(SYS_statx, dir_fd, path, flags, mask, buf));
}

int sys_statfs(const char* path, struct statfs64* buf){
#ifdef SYS_statfs64
	return syscall_error(syscall(SYS_statfs64, path, buf));
#else
	return syscall_error(syscall(SYS_statfs, path, buf));
#endif
}

int sys_fstatfs(int fd, struct statfs64* buf) {
#ifdef SYS_fstatfs64
	return syscall_error(syscall(SYS_fstatfs64, fd, buf));
#else
	return syscall_error(syscall(SYS_fstatfs, fd, buf));
#endif
}

int sys_faccessat(int dir_fd, const char* path, int mode, int flags) {
	return syscall_error(syscall(SYS_faccessat, dir_fd, path, mode, flags));
}

int sys_readlinkat(int dir_fd, const char* path, char* buf, size_t buf_size) {
	return syscall_error(syscall(SYS_readlinkat, dir_fd, path, buf, buf_size));
}

int sys_fchownat(int dir_fd, const char* path, uid_t owner, gid_t group, int flags) {
	return syscall_error(syscall(SYS_fchownat, dir_fd, path, owner, group, flags));
}

int sys_open_dir(const char* path, int* handle) {
	return sys_openat(AT_FDCWD, path, O_RDONLY | O_DIRECTORY | O_CLOEXEC, 0, handle);
}

int sys_read_dir(int handle, void* buffer, size_t max_size, size_t* count_read) {
	auto res = syscall(SYS_getdents64, handle, buffer, static_cast<unsigned int>(max_size));
	if (int err = syscall_error(res)) {
		return err;
	}
	*count_read = static_cast<size_t>(res);
	return 0;
}

int sys_getxattr(const char* path, const char* name, void* value, size_t size, ssize_t* ret) {
	auto res = syscall(SYS_getxattr, path, name, value, size);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<ssize_t>(res);
	return 0;
}

int sys_fgetxattr(int fd, const char* name, void* value, size_t size, ssize_t* ret) {
	auto res = syscall(SYS_fgetxattr, fd, name, value, size);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<ssize_t>(res);
	return 0;
}

int sys_setxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
	return syscall_error(syscall(SYS_setxattr, path, name, value, size, flags));
}

int sys_fsetxattr(int fd, const char* name, const void* value, size_t size, int flags) {
	return syscall_error(syscall(SYS_fsetxattr, fd, name, value, size, flags));
}

int sys_removexattr(const char* path, const char* name) {
	return syscall_error(syscall(SYS_removexattr, path, name));
}

int sys_fremovexattr(int fd, const char* name) {
	return syscall_error(syscall(SYS_removexattr, fd, name));
}

int sys_listxattr(const char* path, char* list, size_t size, ssize_t* ret) {
	auto res = syscall(SYS_listxattr, path, list, size);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<ssize_t>(res);
	return 0;
}

int sys_getcwd(char* buf, size_t size) {
	return syscall_error(syscall(SYS_getcwd, buf, size));
}

int sys_gethostname(char* name, size_t len) {
	utsname buf {};
	if (auto err = sys_uname(&buf)) {
		return err;
	}
	size_t str_len = strlen(buf.nodename);
	if (len < str_len + 1) {
		return ENAMETOOLONG;
	}
	memcpy(name, buf.nodename, str_len + 1);
	return 0;
}

int sys_uname(utsname* buf) {
	return syscall_error(syscall(SYS_uname, buf));
}

int sys_chroot(const char* path) {
	return syscall_error(syscall(SYS_chroot, path));
}

int sys_chdir(const char* path) {
	return syscall_error(syscall(SYS_chdir, path));
}

int sys_fchdir(int fd) {
	return syscall_error(syscall(SYS_fchdir, fd));
}

int sys_getpagesize() {
	return 0x1000;
}

#ifdef __i386__

struct user_desc {
	unsigned int entry_number;
	unsigned int base_addr;
	unsigned int limit;
	unsigned int seg_32bit : 1;
	unsigned int contents : 2;
	unsigned int read_exec_only : 1;
	unsigned int limit_in_pages : 1;
	unsigned int seg_not_present : 1;
	unsigned int useable : 1;
};

#endif

#define ARCH_SET_FS 0x1002

int sys_tcb_set(void* tcb) {
#ifdef __x86_64__
	return syscall_error(syscall(SYS_arch_prctl, ARCH_SET_FS, tcb));
#elif defined(__i386__)
	user_desc desc {
		.entry_number = -1u,
		.base_addr = reinterpret_cast<uintptr_t>(tcb),
		.limit = 0xFFFFF,
		.seg_32bit = 1,
		.contents = 0,
		.read_exec_only = 0,
		.limit_in_pages = 1,
		.seg_not_present = 0,
		.useable = 1
	};
	if (auto err = syscall_error(syscall(SYS_set_thread_area, &desc))) {
		return err;
	}
	asm volatile("mov %w0, %%gs" : : "q"(desc.entry_number * 8 + 3));
	return 0;
#else
	#error missing architecture specific code
#endif
}

int sys_capget(cap_user_header_t hdr, cap_user_data_t data) {
	return syscall_error(syscall(SYS_capget, hdr, data));
}

int sys_capset(cap_user_header_t hdr, cap_user_data_t data) {
	return syscall_error(syscall(SYS_capset, hdr, data));
}

int sys_setgroups(size_t size, const gid_t* list) {
	return syscall_error(syscall(SYS_setgroups, size, list));
}

int sys_getrandom(void* buffer, size_t size, unsigned int flags) {
	return syscall_error(syscall(SYS_getrandom, buffer, size, flags));
}

int sys_execve(const char* path, char* const argv[], char* const envp[]) {
	return syscall_error(syscall(SYS_execve, path, argv, envp));
}

int sys_sched_yield() {
	return syscall_error(syscall(SYS_sched_yield));
}

int sys_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5) {
	return syscall_error(syscall(SYS_prctl, option, arg2, arg3, arg4, arg5));
}

int sys_setuid(uid_t uid) {
	return syscall_error(syscall(SYS_setuid, uid));
}

int sys_setgid(gid_t gid) {
	return syscall_error(syscall(SYS_setgid, gid));
}

int sys_waitpid(pid_t pid, int* status, int options, rusage* usage, pid_t* ret) {
	auto res = syscall(SYS_wait4, pid, status, options, usage);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<pid_t>(res);
	return 0;
}

int sys_fork(pid_t* ret) {
	auto res = syscall(SYS_clone, SIGCHLD);
	if (int err = syscall_error(res)) {
		return err;
	}
	*ret = static_cast<pid_t>(res);
	return 0;
}

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_PRIVATE_FLAG 0x80

int sys_futex_wait(int* addr, int value, const timespec* timeout) {
	return syscall_error(syscall(
		SYS_futex,
		reinterpret_cast<uint32_t*>(addr),
		FUTEX_WAIT | FUTEX_PRIVATE_FLAG,
		value,
		timeout));
}

int sys_futex_wake(int* addr) {
	return syscall_error(syscall(
		SYS_futex,
		reinterpret_cast<uint32_t*>(addr),
		FUTEX_WAKE | FUTEX_PRIVATE_FLAG,
		1));
}

int sys_get_thread_id() {
	auto res = syscall(SYS_gettid);
	__ensure(!syscall_error(res));
	return static_cast<int>(res);
}

int sys_get_process_id() {
	auto res = syscall(SYS_getpid);
	__ensure(!syscall_error(res));
	return static_cast<int>(res);
}

int sys_sigprocmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old) {
	return syscall_error(syscall(SYS_rt_sigprocmask, how, set, old, size_t {_NSIG / 8}));
}

#ifdef __x86_64__

asm(R"(
.pushsection .text
.hidden sys_signal_restore_rt
.hidden sys_signal_restore
.type sys_signal_restore_rt, @function
.type sys_signal_restore, @function
sys_signal_restore_rt:
sys_signal_restore:
	mov $15, %rax
	syscall
	ud2
.popsection
)");

#elif defined(__i386__)

asm(R"(
.pushsection .text
.hidden sys_signal_restore_rt
.type sys_signal_restore_rt, @function
sys_signal_restore_rt:
	mov $173, %eax
	int $0x80
	ud2

.hidden sys_signal_restore
.type sys_signal_restore, @function
sys_signal_restore:
	add $8, %esp
	mov $119, %eax
	int $0x80
	ud2
.popsection
)");

#else
#error missing architecture specific code
#endif

extern "C" void sys_signal_restore();
extern "C" void sys_signal_restore_rt();

int sys_sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old) {
	struct KernelSigAction {
		void (*handler)(int);
		unsigned long flags;
		void (*restorer)();
		uint8_t mask[_NSIG / 8];
	};

	KernelSigAction kernel_action {};
	KernelSigAction kernel_old {};
	if (action) {
		kernel_action.handler = action->sa_handler;
		kernel_action.flags = action->sa_flags | SA_RESTORER;
		kernel_action.restorer = (action->sa_flags & SA_SIGINFO) ? sys_signal_restore_rt : sys_signal_restore;
		memcpy(kernel_action.mask, &action->sa_mask, sizeof(kernel_action.mask));
	}

	auto res = syscall(
		SYS_rt_sigaction,
		sig_num,
		action ? &kernel_action : nullptr,
		old ? &kernel_old : nullptr,
		size_t {_NSIG / 8});
	if (int err = syscall_error(res)) {
		return err;
	}

	if (old) {
		old->sa_handler = kernel_old.handler;
		old->sa_flags = kernel_old.flags;
		old->sa_restorer = kernel_old.restorer;
		memcpy(&old->sa_mask, kernel_old.mask, sizeof(kernel_old.mask));
	}

	return 0;
}

int sys_kill(pid_t pid, int sig) {
	return syscall_error(syscall(SYS_kill, pid, sig));
}

int sys_clock_gettime(clockid_t id, timespec* tp) {
	struct KernelTimespec {
		long long tv_sec;
		long long tv_nsec;
	};

	KernelTimespec kernel {};
	if (int err = syscall_error(syscall(SYS_clock_gettime, id, &kernel))) {
		return err;
	}
	tp->tv_sec = static_cast<decltype(tp->tv_sec)>(kernel.tv_sec);
	tp->tv_nsec = static_cast<decltype(tp->tv_nsec)>(kernel.tv_nsec);
	return 0;
}

int sys_thread_create(
	void* stack_base,
	size_t stack_size,
	void* (start_fn)(void* arg),
	void* arg,
	void* tp,
	pid_t* tid) {
#ifdef __i386__
	uint32_t gs;
	asm volatile("mov %%gs, %w0" : "=q"(gs));

	user_desc desc {
		.entry_number = (gs & 0xFFFF) >> 3,
		.base_addr = reinterpret_cast<uintptr_t>(tp),
		.limit = 0xFFFFF,
		.seg_32bit = 1,
		.contents = 0,
		.read_exec_only = 0,
		.limit_in_pages = 1,
		.seg_not_present = 0,
		.useable = 1
	};
	tp = &desc;
#endif

	bool mmap_stack = false;
	if (!stack_base) {
		if (auto err = sys_mmap(nullptr, stack_size + 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0, &stack_base)) {
			return err;
		}
		__ensure(sys_mprotect(stack_base, 0x1000, PROT_NONE) == 0);
		mmap_stack = true;
	}

	auto* stack_ptr = reinterpret_cast<unsigned long*>(reinterpret_cast<uintptr_t>(stack_base) + stack_size + 0x1000);
	*--stack_ptr = reinterpret_cast<uintptr_t>(arg);
	*--stack_ptr = reinterpret_cast<uintptr_t>(start_fn);

	unsigned long flags =
		CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD |
		CLONE_SYSVSEM | CLONE_SETTLS | CLONE_PARENT_SETTID;
	long res;
#ifdef __x86_64__
	register long r10 asm("r10") = reinterpret_cast<long>(nullptr);
	register long r8 asm("r8") = reinterpret_cast<long>(tp);
	asm volatile("syscall; test %%rax, %%rax; jne 1f; pop %%rdi; pop %%rsi; jmp hzlibc_thread_entry; 1:"
		: "=a"(res) :
		"a"(SYS_clone),
		"D"(flags),
		"S"(stack_ptr),
		"d"(tid),
		"r"(r10),
		"r"(r8) :
		"r9", "rcx", "r11", "memory");
#elif defined(__i386__)
	asm volatile("int $0x80; test %%eax, %%eax; jne 1f; call hzlibc_thread_entry; 1:"
		: "=a"(res) :
		"a"(SYS_clone),
		"b"(flags),
		"c"(stack_ptr),
		"d"(tid),
		"S"(tp) :
		"memory");
#else
#error missing architecture specific code
#endif
	if (int err = syscall_error(res)) {
		if (mmap_stack) {
			__ensure(sys_munmap(stack_base, stack_size + 0x1000) == 0);
		}
		return err;
	}

	return 0;
}
