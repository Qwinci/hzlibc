#include "sys.hpp"
#include "log.hpp"
#include "errno.h"

#define STUB_ENOSYS println(__func__, " is a not implemented and returns ENOSYS"); return ENOSYS

int sys_sleep(const timespec64* duration, timespec64* rem) {
	STUB_ENOSYS;
}

int sys_system(const char* cmd) {
	STUB_ENOSYS;
}

void sys_libc_log(hz::string_view str) {
	
}

[[noreturn]] void sys_exit(int status) {
	__builtin_trap();
}

[[noreturn]] void sys_exit_thread() {
	__builtin_trap();
}

int sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off64_t offset, void** ret) {
	STUB_ENOSYS;
}

int sys_munmap(void* addr, size_t length) {
	STUB_ENOSYS;
}

int sys_mremap(void* old_addr, size_t old_size, size_t new_size, int flags, void* new_addr, void** ret) {
	STUB_ENOSYS;
}

int sys_mprotect(void* addr, size_t length, int prot) {
	STUB_ENOSYS;
}

int sys_madvise(void* addr, size_t length, int advice) {
	STUB_ENOSYS;
}

int sys_mlock(const void* addr, size_t length) {
	STUB_ENOSYS;
}

int sys_munlock(const void* addr, size_t length) {
	STUB_ENOSYS;
}

int sys_msync(void* addr, size_t length, int flags) {
	STUB_ENOSYS;
}

int sys_mincore(void* addr, size_t size, unsigned char* vec) {
	STUB_ENOSYS;
}

int sys_klogctl(int type, char* buf, int len, int* ret) {
	STUB_ENOSYS;
}

int sys_shmat(int shm_id, const void* shm_addr, int shm_flag, void** ret) {
	STUB_ENOSYS;
}

int sys_shmdt(const void* shm_addr) {
	STUB_ENOSYS;
}

int sys_shmget(key_t key, size_t size, int shm_flag, int* ret) {
	STUB_ENOSYS;
}

int sys_shmctl(int shm_id, int op, shmid_ds* buf, int* ret) {
	STUB_ENOSYS;
}

int get_shm_name(const char* name, char* buf) {
	STUB_ENOSYS;
}

int sys_shm_open(const char* name, int oflag, mode_t mode, int* ret) {
	STUB_ENOSYS;
}

int sys_shm_unlink(const char* name) {
	STUB_ENOSYS;
}

int sys_openat(int dir_fd, const char* path, int flags, mode_t mode, int* ret) {
	STUB_ENOSYS;
}

int sys_close(int fd) {
	STUB_ENOSYS;
}

int sys_read(int fd, void* buf, size_t count, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_write(int fd, const void* buf, size_t count, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_lseek(int fd, off64_t offset, int whence, off64_t* ret) {
	STUB_ENOSYS;
}

int sys_remove(const char* path) {
	STUB_ENOSYS;
}

int sys_ioctl(int fd, unsigned long op, void* arg, int* ret) {
	STUB_ENOSYS;
}

int sys_mount(const char* source, const char* target, const char* fs_type, unsigned long flags, const void* data) {
	STUB_ENOSYS;
}

int sys_umount2(const char* target, int flags) {
	STUB_ENOSYS;
}

int sys_fcntl(int fd, int cmd, va_list args, int* ret) {
	STUB_ENOSYS;
}

int sys_fadvise(int fd, off64_t offset, off64_t len, int advice) {
	STUB_ENOSYS;
}

int sys_isatty(int fd) {
	STUB_ENOSYS;
}

int sys_ttyname(int fd, char* buffer, size_t size) {
	STUB_ENOSYS;
}

int sys_ptsname(int fd, char* buffer, size_t size) {
	STUB_ENOSYS;
}

int sys_unlockpt(int fd) {
	STUB_ENOSYS;
}

int sys_getpgid(pid_t pid, pid_t* ret) {
	STUB_ENOSYS;
}

int sys_pipe2(int pipe_fd[2], int flags) {
	STUB_ENOSYS;
}

int sys_copy_file_range(
	int fd_in,
	off64_t* off_in,
	int fd_out,
	off64_t* off_out,
	size_t len,
	unsigned int flags,
	ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_close_range(unsigned int first, unsigned int last, int flags) {
	STUB_ENOSYS;
}

int sys_sendfile(int out_fd, int in_fd, off64_t* offset, size_t count, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_flock(int fd, int operation) {
	STUB_ENOSYS;
}

int sys_select(
	int num_fds,
	fd_set* __restrict read_fds,
	fd_set* __restrict write_fds,
	fd_set* __restrict except_fds,
	timeval* __restrict timeout,
	int* ret) {
	STUB_ENOSYS;
}

int sys_pselect(
	int num_fds,
	fd_set* __restrict read_fds,
	fd_set* __restrict write_fds,
	fd_set* __restrict except_fds,
	const timespec64* __restrict timeout,
	const sigset_t* __restrict sig_mask,
	int* ret) {
	STUB_ENOSYS;
}

int sys_readv(int fd, const iovec* iov, int iov_count, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_writev(int fd, const iovec* iov, int iov_count, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_process_vm_readv(
	pid_t pid,
	const iovec* local_iov,
	unsigned long local_iov_count,
	const iovec* remote_iov,
	unsigned long remote_iov_count,
	unsigned long flags,
	ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_process_vm_writev(
	pid_t pid,
	const iovec* local_iov,
	unsigned long local_iov_count,
	const iovec* remote_iov,
	unsigned long remote_iov_count,
	unsigned long flags,
	ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_swapon(const char* path, int flags) {
	STUB_ENOSYS;
}

int sys_swapoff(const char* path) {
	STUB_ENOSYS;
}

int sys_pread(int fd, void* buf, size_t count, off64_t offset, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_pwrite(int fd, const void* buf, size_t count, off64_t offset, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_poll(pollfd* fds, nfds_t num_fds, int timeout, int* ret) {
	STUB_ENOSYS;
}

int sys_ppoll(pollfd* fds, nfds_t num_fds, const timespec64* timeout, const sigset_t* sig_mask, int* ret) {
	STUB_ENOSYS;
}

int sys_eventfd(unsigned int init_value, int flags, int* ret) {
	STUB_ENOSYS;
}

int sys_memfd_create(const char* name, unsigned int flags, int* ret) {
	STUB_ENOSYS;
}

int sys_epoll_create1(int flags, int* ret) {
	STUB_ENOSYS;
}

int sys_epoll_ctl(int epfd, int op, int fd, epoll_event* event) {
	STUB_ENOSYS;
}

int sys_epoll_wait(int epfd, epoll_event* events, int max_events, int timeout, int* ret) {
	STUB_ENOSYS;
}

int sys_signalfd(int fd, const sigset_t* mask, int flags, int* ret) {
	STUB_ENOSYS;
}

int sys_mq_open(const char* name, int oflag, mode_t mode, mq_attr* attr, mqd_t* ret) {
	STUB_ENOSYS;
}

int sys_mq_close(mqd_t mq) {
	STUB_ENOSYS;
}

int sys_mq_unlink(const char* name) {
	STUB_ENOSYS;
}

int sys_stat(StatTarget target, int dir_fd, const char* path, int flags, struct stat64* s) {
	STUB_ENOSYS;
}

int sys_statx(int dir_fd, const char* __restrict path, int flags, unsigned int mask, struct statx* __restrict buf) {
	STUB_ENOSYS;
}

int sys_statfs(const char* path, struct statfs64* buf){
	STUB_ENOSYS;
}

int sys_fstatfs(int fd, struct statfs64* buf) {
	STUB_ENOSYS;
}

mode_t sys_umask(mode_t mask) {
	println("sys_umask is a stub");
	return 0;
}

int sys_faccessat(int dir_fd, const char* path, int mode, int flags) {
	STUB_ENOSYS;
}

int sys_readlinkat(int dir_fd, const char* path, char* buf, size_t buf_size, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_fchownat(int dir_fd, const char* path, uid_t owner, gid_t group, int flags) {
	STUB_ENOSYS;
}

int sys_fchmodat(int dir_fd, const char* path, mode_t mode, int flags) {
	STUB_ENOSYS;
}

int sys_open_dir(const char* path, int* handle) {
	STUB_ENOSYS;
}

int sys_read_dir(int handle, void* buffer, size_t max_size, size_t* count_read) {
	STUB_ENOSYS;
}

int sys_linkat(
	int old_dir_fd,
	const char* old_path,
	int new_dir_fd,
	const char* new_path,
	int flags) {
	STUB_ENOSYS;
}

int sys_symlinkat(const char* target, int new_dir_fd, const char* link_path) {
	STUB_ENOSYS;
}

int sys_unlinkat(int dir_fd, const char* path, int flags) {
	STUB_ENOSYS;
}

int sys_mkdirat(int dir_fd, const char* path, mode_t mode) {
	STUB_ENOSYS;
}

int sys_mknodat(int dir_fd, const char* path, mode_t mode, dev_t dev) {
	STUB_ENOSYS;
}

int sys_utimensat(int dir_fd, const char* path, const timespec64 times[2], int flags) {
	STUB_ENOSYS;
}

int sys_rmdir(const char* path) {
	STUB_ENOSYS;
}

int sys_renameat(int old_dir_fd, const char* old_path, int new_dir_fd, const char* new_path) {
	STUB_ENOSYS;
}

int sys_truncate(const char* path, off64_t length) {
	STUB_ENOSYS;
}

int sys_ftruncate(int fd, off64_t length) {
	STUB_ENOSYS;
}

int sys_dup(int old_fd, int* ret) {
	STUB_ENOSYS;
}

int sys_dup2(int old_fd, int new_fd, int flags) {
	STUB_ENOSYS;
}

int sys_sync() {
	STUB_ENOSYS;
}

int sys_fsync(int fd) {
	STUB_ENOSYS;
}

int sys_fdatasync(int fd) {
	STUB_ENOSYS;
}

int sys_fallocate(int fd, int mode, off64_t offset, off64_t len) {
	STUB_ENOSYS;
}

int sys_getxattr(const char* path, const char* name, void* value, size_t size, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_lgetxattr(const char* path, const char* name, void* value, size_t size, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_fgetxattr(int fd, const char* name, void* value, size_t size, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_setxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
	STUB_ENOSYS;
}

int sys_lsetxattr(const char* path, const char* name, const void* value, size_t size, int flags) {
	STUB_ENOSYS;
}

int sys_fsetxattr(int fd, const char* name, const void* value, size_t size, int flags) {
	STUB_ENOSYS;
}

int sys_removexattr(const char* path, const char* name) {
	STUB_ENOSYS;
}

int sys_lremovexattr(const char* path, const char* name) {
	STUB_ENOSYS;
}

int sys_fremovexattr(int fd, const char* name) {
	STUB_ENOSYS;
}

int sys_listxattr(const char* path, char* list, size_t size, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_llistxattr(const char* path, char* list, size_t size, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_flistxattr(int fd, char* list, size_t size, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_getcwd(char* buf, size_t size) {
	STUB_ENOSYS;
}

int sys_gethostname(char* name, size_t len) {
	STUB_ENOSYS;
}

int sys_uname(utsname* buf) {
	STUB_ENOSYS;
}

int sys_chroot(const char* path) {
	STUB_ENOSYS;
}

int sys_chdir(const char* path) {
	STUB_ENOSYS;
}

int sys_fchdir(int fd) {
	STUB_ENOSYS;
}

int sys_getpagesize() {
	return 0x1000;
}

int sys_sysinfo(struct sysinfo* info) {
	STUB_ENOSYS;
}

int sys_tcb_set(void* tcb) {
	STUB_ENOSYS;
}

int sys_capget(cap_user_header_t hdr, cap_user_data_t data) {
	STUB_ENOSYS;
}

int sys_capset(cap_user_header_t hdr, cap_user_data_t data) {
	STUB_ENOSYS;
}

int sys_getgroups(int size, gid_t* list, int* ret) {
	STUB_ENOSYS;
}

int sys_setgroups(size_t size, const gid_t* list) {
	STUB_ENOSYS;
}

int sys_getrandom(void* buffer, size_t size, unsigned int flags, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_execve(const char* path, char* const argv[], char* const envp[]) {
	STUB_ENOSYS;
}

int sys_sched_yield() {
	STUB_ENOSYS;
}

int sys_sched_getparam(pid_t pid, sched_param* ret) {
	STUB_ENOSYS;
}

int sys_sched_setscheduler(pid_t pid, int policy, const sched_param* param) {
	STUB_ENOSYS;
}

int sys_sched_getscheduler(pid_t pid, int* ret) {
	STUB_ENOSYS;
}

int sys_sched_setaffinity(pid_t pid, size_t cpu_set_size, const cpu_set_t* mask) {
	STUB_ENOSYS;
}

int sys_sched_getaffinity(pid_t pid, size_t cpu_set_size, cpu_set_t* mask) {
	STUB_ENOSYS;
}

int sys_sched_get_priority_min(int policy, int* ret) {
	STUB_ENOSYS;
}

int sys_sched_get_priority_max(int policy, int* ret) {
	STUB_ENOSYS;
}

int sys_sched_getcpu(int* ret) {
	STUB_ENOSYS;
}

int sys_prctl(int option, unsigned long arg2, unsigned long arg3, unsigned long arg4, unsigned long arg5, int* ret) {
	STUB_ENOSYS;
}

int sys_iopl(int level) {
	STUB_ENOSYS;
}

int sys_setuid(uid_t uid) {
	STUB_ENOSYS;
}

int sys_setgid(gid_t gid) {
	STUB_ENOSYS;
}

int sys_setsid(pid_t* ret) {
	STUB_ENOSYS;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	STUB_ENOSYS;
}

int sys_waitpid(pid_t pid, int* status, int options, rusage* usage, pid_t* ret) {
	STUB_ENOSYS;
}

int sys_waitid(idtype_t id_type, id_t id, siginfo_t* info, int options) {
	STUB_ENOSYS;
}

int sys_fork(pid_t* ret) {
	STUB_ENOSYS;
}

int sys_unshare(int flags) {
	STUB_ENOSYS;
}

int sys_getpriority(int which, id_t who, int* ret) {
	STUB_ENOSYS;
}

int sys_setpriority(int which, id_t who, int prio) {
	STUB_ENOSYS;
}

int sys_getrlimit(int resource, rlimit64* rlim) {
	STUB_ENOSYS;
}

int sys_setrlimit(int resource, const rlimit64* rlim) {
	STUB_ENOSYS;
}

int sys_getrusage(int who, rusage* usage) {
	STUB_ENOSYS;
}

int sys_personality(unsigned long persona, int* ret) {
	STUB_ENOSYS;
}

int sys_ptrace(__ptrace_request op, pid_t pid, void* addr, void* data, long* ret) {
	STUB_ENOSYS;
}

int sys_setns(int fd, int ns_type) {
	STUB_ENOSYS;
}

int sys_sysconf(int num, long* ret) {
	STUB_ENOSYS;
}

int sys_futex_wait(int* addr, int value, const timespec* timeout, bool pshared) {
	STUB_ENOSYS;
}

int sys_futex_wake(int* addr, bool pshared) {
	STUB_ENOSYS;
}

int sys_futex_wake_all(int* addr, bool pshared) {
	STUB_ENOSYS;
}

pid_t sys_get_thread_id() {
	println("sys_get_thread_id is a stub");
	return 0;
}

pid_t sys_get_process_id() {
	println("sys_get_process_id is a stub");
	return 0;
}

pid_t sys_getppid() {
	println("sys_getppid is a stub");
	return 0;
}

uid_t sys_getuid() {
	println("sys_getuid is a stub");
	return 0;
}

uid_t sys_geteuid() {
	println("sys_geteuid is a stub");
	return 0;
}

gid_t sys_getgid() {
	println("sys_getgid is a stub");
	return 0;
}

gid_t sys_getegid() {
	println("sys_getegid is a stub");
	return 0;
}

int sys_getsid(pid_t pid, pid_t* ret) {
	STUB_ENOSYS;
}

int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	STUB_ENOSYS;
}

int sys_getresuid(uid_t* ruid, uid_t* euid, uid_t* suid) {
	STUB_ENOSYS;
}

int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	STUB_ENOSYS;
}

int sys_getresgid(gid_t* rgid, gid_t* egid, gid_t* sgid) {
	STUB_ENOSYS;
}

int sys_semget(key_t key, int num_sems, int sem_flag, int* ret) {
	STUB_ENOSYS;
}

int sys_semctl(int sem_id, int sem_num, int op, void* arg, int* ret) {
	STUB_ENOSYS;
}

int sys_semop(int sem_id, sembuf* sops, size_t num_sops) {
	STUB_ENOSYS;
}

int sys_sigprocmask(int how, const sigset_t* __restrict set, sigset_t* __restrict old) {
	STUB_ENOSYS;
}

int sys_sigaction(int sig_num, const struct sigaction* __restrict action, struct sigaction* __restrict old) {
	STUB_ENOSYS;
}

int sys_sigtimedwait(const sigset_t* __restrict set, siginfo_t* __restrict info, const timespec* timeout, int* ret) {
	STUB_ENOSYS;
}

int sys_raise(int sig_num) {
	STUB_ENOSYS;
}

int sys_sigaltstack(const stack_t* stack, stack_t* old_stack) {
	STUB_ENOSYS;
}

int sys_sigsuspend(const sigset_t* set) {
	STUB_ENOSYS;
}

int sys_sigpending(sigset_t* set) {
	STUB_ENOSYS;
}

int sys_kill(pid_t pid, int sig) {
	STUB_ENOSYS;
}

int sys_tgkill(pid_t pid, pid_t tid, int sig) {
	STUB_ENOSYS;
}

int sys_clock_gettime(clockid_t id, timespec* tp) {
	STUB_ENOSYS;
}

int sys_clock_settime(clockid_t id, const timespec* tp) {
	STUB_ENOSYS;
}

int sys_clock_getres(clockid_t id, timespec* res) {
	STUB_ENOSYS;
}

int sys_clock_nanosleep(clockid_t id, int flags, const timespec64* req, timespec64* rem) {
	STUB_ENOSYS;
}

unsigned int sys_alarm(unsigned int seconds) {
	println("sys_alarm is a stub");
	return 0;
}

int sys_inotify_init1(int flags, int* ret) {
	STUB_ENOSYS;
}

int sys_inotify_add_watch(int fd, const char* path, uint32_t mask, int* ret) {
	STUB_ENOSYS;
}

int sys_inotify_rm_watch(int fd, int wd) {
	STUB_ENOSYS;
}

int sys_socket(int domain, int type, int protocol, int* ret) {
	STUB_ENOSYS;
}

int sys_socketpair(int domain, int type, int protocol, int sv[2]) {
	STUB_ENOSYS;
}

int sys_connect(int fd, const sockaddr* addr, socklen_t addr_len) {
	STUB_ENOSYS;
}

int sys_bind(int fd, const sockaddr* addr, socklen_t addr_len) {
	STUB_ENOSYS;
}

int sys_listen(int fd, int backlog) {
	STUB_ENOSYS;
}

int sys_accept4(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len, int flags, int* ret) {
	STUB_ENOSYS;
}

int sys_shutdown(int fd, int how) {
	STUB_ENOSYS;
}

int sys_getsockopt(int fd, int level, int option, void* __restrict value, socklen_t* __restrict value_len) {
	STUB_ENOSYS;
}

int sys_setsockopt(int fd, int level, int option, const void* value, socklen_t value_len) {
	STUB_ENOSYS;
}

int sys_getsockname(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len) {
	STUB_ENOSYS;
}

int sys_getpeername(int fd, sockaddr* __restrict addr, socklen_t* __restrict addr_len) {
	STUB_ENOSYS;
}

int sys_sendto(
	int fd,
	const void* buf,
	size_t len,
	int flags,
	const sockaddr* dest_addr,
	socklen_t addr_len,
	ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_sendmsg(int fd, const msghdr* msg, int flags, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_recvfrom(
	int fd,
	void* __restrict buf,
	size_t len,
	int flags,
	sockaddr* __restrict src_addr,
	socklen_t* addr_len,
	ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_recvmsg(int fd, msghdr* msg, int flags, ssize_t* ret) {
	STUB_ENOSYS;
}

int sys_thread_create(
	void* stack_base,
	size_t stack_size,
	void* (start_fn)(void* arg),
	void* arg,
	void* tp,
	pid_t* tid) {
	STUB_ENOSYS;
}

int sys_thread_set_name(pid_t tid, const char* name) {
	STUB_ENOSYS;
}

int sys_thread_get_name(pid_t tid, char* name, size_t len) {
	STUB_ENOSYS;
}
