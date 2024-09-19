#pragma once
#include <stddef.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/capability.h>
#include <sys/resource.h>
#include <sys/statfs.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/epoll.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sched.h>
#include <ucontext.h>
#include <pty.h>
#include <mqueue.h>
#include <hz/string_view.hpp>

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
int sys_mremap(void* old_addr, size_t old_size, size_t new_size, int flags, void* new_addr, void** ret);
int sys_mprotect(void* addr, size_t length, int prot);
int sys_madvise(void* addr, size_t length, int advice);
int sys_mlock(const void* addr, size_t length);
int sys_munlock(const void* addr, size_t length);
int sys_msync(void* addr, size_t length, int flags);
int sys_mincore(void* addr, size_t size, unsigned char* vec);
int sys_klogctl(int type, char* buf, int len, int* ret);

int sys_shmat(int shm_id, const void* shm_addr, int shm_flag, void** ret);
int sys_shmdt(const void* shm_addr);
int sys_shmget(key_t key, size_t size, int shm_flag, int* ret);
int sys_shmctl(int shm_id, int op, shmid_ds* buf, int* ret);
int sys_shm_open(const char* name, int oflag, mode_t mode, int* ret);
int sys_shm_unlink(const char* name);

int sys_openat(int dir_fd, const char* path, int flags, mode_t mode, int* ret);
int sys_close(int fd);
int sys_read(int fd, void* buf, size_t count, ssize_t* ret);
int sys_write(int fd, const void* buf, size_t count, ssize_t* ret);
int sys_lseek(int fd, off64_t offset, int whence, off64_t* ret);
int sys_ioctl(int fd, unsigned long op, void* arg, int* ret);
int sys_mount(const char* source, const char* target, const char* fs_type, unsigned long flags, const void* data);
int sys_umount2(const char* target, int flags);
int sys_fcntl(int fd, int cmd, va_list args, int* ret);
int sys_fadvise(int fd, off64_t offset, off64_t len, int advice);
int sys_isatty(int fd);
int sys_ttyname(int fd, char* buffer, size_t size);
int sys_ptsname(int fd, char* buffer, size_t size);
int sys_unlockpt(int fd);
int sys_getpgid(pid_t pid, pid_t* ret);
int sys_pipe2(int pipe_fd[2], int flags);
int sys_copy_file_range(
	int fd_in,
	off64_t* off_in,
	int fd_out,
	off64_t* off_out,
	size_t len,
	unsigned int flags,
	ssize_t* ret);
int sys_close_range(unsigned int first, unsigned int last, int flags);
int sys_sendfile(int out_fd, int in_fd, off64_t* offset, size_t count, ssize_t* ret);
int sys_flock(int fd, int operation);
int sys_select(
	int num_fds,
	fd_set* __restrict read_fds,
	fd_set* __restrict write_fds,
	fd_set* __restrict except_fds,
	timeval* __restrict timeout,
	int* ret);
int sys_pselect(
	int num_fds,
	fd_set* __restrict read_fds,
	fd_set* __restrict write_fds,
	fd_set* __restrict except_fds,
	const timespec64* __restrict timeout,
	const sigset_t* __restrict sig_mask,
	int* ret);
int sys_readv(int fd, const iovec* iov, int iov_count, ssize_t* ret);
int sys_writev(int fd, const iovec* iov, int iov_count, ssize_t* ret);
int sys_process_vm_readv(
	pid_t pid,
	const iovec* local_iov,
	unsigned long local_iov_count,
	const iovec* remote_iov,
	unsigned long remote_iov_count,
	unsigned long flags,
	ssize_t* ret);
int sys_process_vm_writev(
	pid_t pid,
	const iovec* local_iov,
	unsigned long local_iov_count,
	const iovec* remote_iov,
	unsigned long remote_iov_count,
	unsigned long flags,
	ssize_t* ret);
int sys_swapon(const char* path, int flags);
int sys_swapoff(const char* path);

int sys_pread(int fd, void* buf, size_t count, off64_t offset, ssize_t* ret);
int sys_pwrite(int fd, const void* buf, size_t count, off64_t offset, ssize_t* ret);

int sys_poll(pollfd* fds, nfds_t num_fds, int timeout, int* ret);
int sys_ppoll(pollfd* fds, nfds_t num_fds, const timespec64* timeout, const sigset_t* sig_mask, int* ret);

int sys_eventfd(unsigned int init_value, int flags, int* ret);
int sys_memfd_create(const char* name, unsigned int flags, int* ret);
int sys_epoll_create1(int flags, int* ret);
int sys_epoll_ctl(int epfd, int op, int fd, epoll_event* event);
int sys_epoll_wait(int epfd, epoll_event* events, int max_events, int timeout, int* ret);
int sys_signalfd(int fd, const sigset_t* mask, int flags, int* ret);
int sys_mq_open(const char* name, int oflag, mode_t mode, mq_attr* attr, mqd_t* ret);
int sys_mq_close(mqd_t mq);
int sys_mq_unlink(const char* name);

enum class StatTarget {
	Path,
	Fd,
	FdPath
};

int sys_stat(StatTarget target, int dir_fd, const char* path, int flags, struct stat64* s);
int sys_statx(int dir_fd, const char* __restrict path, int flags, unsigned int mask, struct statx* __restrict buf);
int sys_statfs(const char* path, struct statfs64* buf);
int sys_fstatfs(int fd, struct statfs64* buf);
mode_t sys_umask(mode_t mask);

int sys_faccessat(int dir_fd, const char* path, int mode, int flags);
int sys_readlinkat(int dir_fd, const char* path, char* buf, size_t buf_size, ssize_t* ret);
int sys_fchownat(int dir_fd, const char* path, uid_t owner, gid_t group, int flags);
int sys_fchmodat(int dir_fd, const char* path, mode_t mode, int flags);
int sys_open_dir(const char* path, int* handle);
int sys_read_dir(int handle, void* buffer, size_t max_size, size_t* count_read);
int sys_linkat(
	int old_dir_fd,
	const char* old_path,
	int new_dir_fd,
	const char* new_path,
	int flags);
int sys_symlinkat(const char* target, int new_dir_fd, const char* link_path);
int sys_unlinkat(int dir_fd, const char* path, int flags);
int sys_mkdirat(int dir_fd, const char* path, mode_t mode);
int sys_mknodat(int dir_fd, const char* path, mode_t mode, dev_t dev);
int sys_utimensat(int dir_fd, const char* path, const timespec64 times[2], int flags);
int sys_rmdir(const char* path);
int sys_renameat(int old_dir_fd, const char* old_path, int new_dir_fd, const char* new_path);
int sys_truncate(const char* path, off64_t length);
int sys_ftruncate(int fd, off64_t length);
int sys_dup(int old_fd, int* ret);
int sys_dup2(int old_fd, int new_fd, int flags);
int sys_sync();
int sys_fsync(int fd);
int sys_fdatasync(int fd);
int sys_fallocate(int fd, int mode, off64_t offset, off64_t len);

int sys_getxattr(const char* path, const char* name, void* value, size_t size, ssize_t* ret);
int sys_lgetxattr(const char* path, const char* name, void* value, size_t size, ssize_t* ret);
int sys_fgetxattr(int fd, const char* name, void* value, size_t size, ssize_t* ret);
int sys_setxattr(const char* path, const char* name, const void* value, size_t size, int flags);
int sys_lsetxattr(const char* path, const char* name, const void* value, size_t size, int flags);
int sys_fsetxattr(int fd, const char* name, const void* value, size_t size, int flags);
int sys_removexattr(const char* path, const char* name);
int sys_lremovexattr(const char* path, const char* name);
int sys_fremovexattr(int fd, const char* name);
int sys_listxattr(const char* path, char* list, size_t size, ssize_t* ret);
int sys_llistxattr(const char* path, char* list, size_t size, ssize_t* ret);
int sys_flistxattr(int fd, char* list, size_t size, ssize_t* ret);

int sys_getcwd(char* buf, size_t size);
int sys_gethostname(char* name, size_t len);
int sys_uname(utsname* buf);
int sys_chroot(const char* path);
int sys_chdir(const char* path);
int sys_fchdir(int fd);
int sys_getpagesize();
int sys_sysinfo(struct sysinfo* info);

int sys_tcb_set(void* tcb);
int sys_capget(cap_user_header_t hdr, cap_user_data_t data);
int sys_capset(cap_user_header_t hdr, cap_user_data_t data);
int sys_getgroups(int size, gid_t* list, int* ret);
int sys_setgroups(size_t size, const gid_t* list);
int sys_getrandom(void* buffer, size_t size, unsigned int flags, ssize_t* ret);

int sys_execve(const char* path, char* const argv[], char* const envp[]);

int sys_sched_yield();
int sys_sched_getparam(pid_t pid, sched_param* ret);
int sys_sched_setscheduler(pid_t pid, int policy, const sched_param* param);
int sys_sched_getscheduler(pid_t pid, int* ret);
int sys_sched_setaffinity(pid_t pid, size_t cpu_set_size, const cpu_set_t* mask);
int sys_sched_getaffinity(pid_t pid, size_t cpu_set_size, cpu_set_t* mask);
int sys_sched_get_priority_min(int policy, int* ret);
int sys_sched_get_priority_max(int policy, int* ret);
int sys_sched_getcpu(int* ret);

int sys_prctl(int option, unsigned long arg0, unsigned long arg1, unsigned long arg2, unsigned long arg3, int* ret);
int sys_iopl(int level);
int sys_setuid(uid_t uid);
int sys_setgid(gid_t gid);
int sys_setsid(pid_t* ret);
int sys_setpgid(pid_t pid, pid_t pgid);
int sys_waitpid(pid_t pid, int* status, int options, rusage* usage, pid_t* ret);
int sys_waitid(idtype_t id_type, id_t id, siginfo_t* info, int options);
int sys_fork(pid_t* ret);
int sys_unshare(int flags);
int sys_getpriority(int which, id_t who, int* ret);
int sys_setpriority(int which, id_t who, int prio);
int sys_getrlimit(int resource, rlimit64* rlim);
int sys_setrlimit(int resource, const rlimit64* rlim);
int sys_getrusage(int who, rusage* usage);
int sys_personality(unsigned long persona, int* ret);
int sys_ptrace(__ptrace_request op, pid_t pid, void* addr, void* data, long* ret);
int sys_setns(int fd, int ns_type);

int sys_futex_wait(int* addr, int value, const timespec* timeout, bool pshared = false);
int sys_futex_wake(int* addr, bool pshared = false);
int sys_futex_wake_all(int* addr, bool pshared = false);
pid_t sys_get_thread_id();
pid_t sys_get_process_id();
pid_t sys_getppid();
uid_t sys_getuid();
uid_t sys_geteuid();
gid_t sys_getgid();
gid_t sys_getegid();
int sys_getsid(pid_t pid, pid_t* ret);
int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid);
int sys_getresuid(uid_t* ruid, uid_t* euid, uid_t* suid);
int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid);
int sys_getresgid(gid_t* rgid, gid_t* egid, gid_t* sgid);

int sys_semget(key_t key, int num_sems, int sem_flag, int* ret);
int sys_semctl(int sem_id, int sem_num, int op, void* arg, int* ret);
int sys_semop(int sem_id, sembuf* sops, size_t num_sops);

int sys_sigprocmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old);
int sys_sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old);
int sys_sigtimedwait(const sigset_t* __restrict set, siginfo_t* __restrict info, const timespec* timeout, int* ret);
int sys_sigaltstack(const stack_t* stack, stack_t* old_stack);
int sys_sigsuspend(const sigset_t* set);
int sys_sigpending(sigset_t* set);
int sys_kill(pid_t pid, int sig);
int sys_tgkill(pid_t pid, pid_t tid, int sig);

int sys_clock_gettime(clockid_t id, timespec* tp);
int sys_clock_settime(clockid_t id, const timespec* tp);
int sys_clock_getres(clockid_t id, timespec* res);
int sys_clock_nanosleep(clockid_t id, int flags, const timespec64* req, timespec64* rem);
unsigned int sys_alarm(unsigned int seconds);

int sys_inotify_init1(int flags, int* ret);
int sys_inotify_add_watch(int fd, const char* path, uint32_t mask, int* ret);
int sys_inotify_rm_watch(int fd, int wd);

int sys_socket(int domain, int type, int protocol, int* ret);
int sys_socketpair(int domain, int type, int protocol, int sv[2]);
int sys_connect(int fd, const sockaddr* addr, socklen_t addr_len);
int sys_bind(int fd, const sockaddr* addr, socklen_t addr_len);
int sys_listen(int fd, int backlog);
int sys_accept4(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len, int flags, int* ret);
int sys_shutdown(int fd, int how);

int sys_getsockopt(int fd, int level, int option, void* __restrict value, socklen_t* __restrict value_len);
int sys_setsockopt(int fd, int level, int option, const void* value, socklen_t value_len);
int sys_getsockname(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len);
int sys_getpeername(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len);

int sys_sendto(
	int fd,
	const void* buf,
	size_t len,
	int flags,
	const sockaddr* dest_addr,
	socklen_t addr_len,
	ssize_t* ret);
int sys_sendmsg(int fd, const msghdr* msg, int flags, ssize_t* ret);
int sys_recvfrom(
	int fd,
	void* __restrict buf,
	size_t len,
	int flags,
	sockaddr* __restrict src_addr,
	socklen_t* addr_len,
	ssize_t* ret);
int sys_recvmsg(int fd, msghdr* msg, int flags, ssize_t* ret);

int sys_thread_create(
	void* stack_base,
	size_t stack_size,
	void* (start_fn)(void* arg),
	void* arg,
	void* tp,
	pid_t* tid);
int sys_thread_set_name(pid_t tid, const char* name);
int sys_thread_get_name(pid_t tid, char* name, size_t len);
