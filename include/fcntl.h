#ifndef _FCNTL_H
#define _FCNTL_H

#include <bits/utils.h>
#include <stdlib.h>
#include <sys/types.h>

__begin

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_CREAT 0x40
#define O_EXCL 0x80
#define O_NOCTTY 0x100
#define O_TRUNC 0x200
#define O_APPEND 0x400
#define O_NONBLOCK 0x800
#define O_DSYNC 0x10000
#define FASYNC 0x2000
#define O_DIRECT 0x4000
#define O_LARGEFILE 0x8000
#define O_DIRECTORY 0x10000
#define O_NOFOLLOW 0x20000
#define O_NOATIME 0x40000
#define O_CLOEXEC 0x80000
#define __O_TMPFILE 0x400000
#define O_TMPFILE (__O_TMPFILE | O_DIRECTORY)

#define AT_FDCWD -100

#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_EACCESS 0x200
#define AT_EMPTY_PATH 0x1000

#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define F_GETLK 5
#define F_SETLK 6
#define F_SETLKW 7
#define F_SETOWN 8
#define F_GETOWN 9
#define F_SETSIG 10
#define F_GETSIG 11

#if UINTPTR_MAX == UINT32_MAX
#define F_GETLK64 12
#define F_SETLK64 13
#define F_SETLKW64 14
#else
#define F_GETLK64 F_GETLK
#define F_SETLK64 F_SETLK
#define F_SETLKW64 F_SETLKW
#endif

#define F_SETOWN_EX 15
#define F_GETOWN_EX 16

#define F_GETOWNER_UIDS 17
#define F_OFD_GETLK 36
#define F_OFD_SETLK 37
#define F_OFD_SETLKW 38

#define FD_CLOEXEC 1

#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

#define F_EXLCK 4
#define F_SHLCK 8

#define LOCK_SH 1
#define LOCK_EX 2
#define LOCK_NB 4
#define LOCK_UN 8

struct flock {
	short l_type;
	short l_whence;
	off_t l_start;
	off_t l_len;
	pid_t l_pid;
};

struct flock64 {
	short l_type;
	short l_whence;
	off64_t l_start;
	off64_t l_len;
	pid_t l_pid;
};

int open(const char* __path, int __flags, ...);
int open64(const char* __path, int __flags, ...);
int openat(int __dir_fd, const char* __path, int __flags, ...);
int fcntl(int __fd, int __cmd, ...);
int fcntl64(int __fd, int __cmd, ...);
int posix_fadvise(int __fd, off_t __offset, off_t __len, int __advice);
int posix_fallocate(int __fd, off_t __offset, off_t __len);
int posix_fallocate64(int __fd, off64_t __offset, off64_t __len);

__end

#endif
