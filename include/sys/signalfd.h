#ifndef _SYS_SIGNALFD_H
#define _SYS_SIGNALFD_H

#include <bits/utils.h>
#include <signal.h>

__begin_decls

#define SFD_NONBLOCK 800
#define SFD_CLOEXEC 0x80000

struct signalfd_siginfo {
	uint32_t ssi_signo;
	int32_t ssi_errno;
	int32_t ssi_code;
	uint32_t ssi_pid;
	uint32_t ssi_uid;
	int32_t ssi_fd;
	uint32_t ssi_tid;
	uint32_t ssi_band;
	uint32_t ssi_overrun;
	uint32_t ssi_trapno;
	int32_t ssi_status;
	int32_t ssi_int;
	uint64_t ssi_ptr;
	uint64_t ssi_utime;
	uint64_t ssi_stime;
	uint64_t ssi_addr;
	uint16_t ssi_addr_lsb;
	uint16_t __unused0;
	int32_t ssi_syscall;
	uint64_t ssi_call_addr;
	uint32_t ssi_arch;
	uint8_t __unused1[28];
};

int signalfd(int __fd, const sigset_t* __mask, int __flags);

__end_decls

#endif
