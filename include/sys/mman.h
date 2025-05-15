#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <bits/utils.h>
#include <sys/types.h>

__begin_decls

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4

#define MAP_SHARED 1
#define MAP_PRIVATE 2

#define MAP_FILE 0
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS
#define MAP_32BIT 0x40
#define MAP_NORESERVE 0x4000

#define MAP_FAILED ((void*) -1)

#define MS_ASYNC 1
#define MS_INVALIDATE 2
#define MS_SYNC 4

#define MADV_NORMAL 0
#define MADV_RANDOM 1
#define MADV_SEQUENTIAL 2
#define MADV_WILLNEED 3
#define MADV_DONTNEED 4

void* mmap(void* __addr, size_t __length, int __prot, int __flags, int __fd, off_t __offset);
void* mmap64(void* __addr, size_t __length, int __prot, int __flags, int __fd, off64_t __offset);
int munmap(void* __addr, size_t __length);
int mprotect(void* __addr, size_t __length, int __prot);
int madvise(void* __addr, size_t __length, int __advice);
int posix_madvise(void* __addr, size_t __length, int __advice);
int mlock(const void* __addr, size_t __length);
int munlock(const void* __addr, size_t __length);
int msync(void* __addr, size_t __length, int __flags);

int shm_open(const char* __name, int __oflag, mode_t __mode);
int shm_unlink(const char* __name);

// linux

#define MREMAP_FIXED 2

int memfd_create(const char* __name, unsigned int __flags);
void* mremap(void* __old_addr, size_t __old_size, size_t __new_size, int __flags, ...);
int mincore(void* __addr, size_t __size, unsigned char* __vec);

__end_decls

#endif
