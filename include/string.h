#ifndef _STRING_H
#define _STRING_H

#include "bits/utils.h"
#include <stddef.h>

__begin

size_t strlen(const char* __str);
void* memset(void* __dest, int __ch, size_t __size);
void* memcpy(void* __restrict __dest, const void* __restrict __src, size_t __size);
int memcmp(const void* __lhs, const void* __rhs, size_t __count);
void* memchr(const void* __ptr, int __ch, size_t __count);
void* memmove(void* __dest, const void* __src, size_t __size);

int strcmp(const char* __lhs, const char* __rhs);
int strncmp(const char* __lhs, const char* __rhs, size_t __count);
char* strchr(const char* __str, int __ch);
char* strrchr(const char* __str, int __ch);
char* strcpy(char* __restrict __dest, const char* __restrict __src);
char* strncpy(char* __restrict __dest, const char* __restrict __src, size_t __count);
char* strdup(const char* __str);
size_t strspn(const char* __str, const char* __search);
size_t strcspn(const char* __str, const char* __search);
int strcoll(const char* __lhs, const char* __rhs);

char* strerror(int __err_num);

// posix
size_t strnlen(const char* __str, size_t __max_len);
char* stpcpy(char* __restrict __dest, const char* __src);

// glibc
void* rawmemchr(const void* __ptr, int __ch);
void* mempcpy(void* __restrict __dest, const void* __restrict __src, size_t __size);
void* memrchr(const void* __ptr, int __ch, size_t __count);

__end

#endif
