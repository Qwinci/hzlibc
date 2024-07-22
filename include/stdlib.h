#ifndef _STDLIB_H
#define _STDLIB_H

#include "bits/utils.h"
#include <stddef.h>

__begin

void* malloc(size_t __size);
void* realloc(void* __old, size_t __new_size);
void* aligned_alloc(size_t __alignment, size_t __size);
void* calloc(size_t __num, size_t __size);
void free(void* __ptr);

char* getenv(const char* __name);

__attribute__((__noreturn__)) void exit(int __status);
__attribute__((__noreturn__)) void abort(void);
int atexit(void (*__func)(void));

int atoi(const char* __str);
unsigned long strtoul(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);

void qsort(void* __ptr, size_t __count, size_t __size, int (*__comp)(const void* __a, const void* __b));

// posix
int setenv(const char* __name, const char* __value, int __overwrite);
int unsetenv(const char* __name);

// glibc
void* reallocarray(void* __old, size_t __num_blocks, size_t __size);
int clearenv(void);

__end

#endif
