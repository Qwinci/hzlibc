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

#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_ANON MAP_ANONYMOUS

#define MAP_FAILED ((void*) -1)

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

__end_decls

#endif
