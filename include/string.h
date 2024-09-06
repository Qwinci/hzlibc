#ifndef _STRING_H
#define _STRING_H

#include "bits/utils.h"
#include <stddef.h>
#include <locale.h>

__begin_decls

size_t strlen(const char* __str);
void* memset(void* __dest, int __ch, size_t __size);
void* memcpy(void* __restrict __dest, const void* __restrict __src, size_t __size);
void* memccpy(void* __restrict __dest, const void* __restrict __src, int __ch, size_t __size);
int memcmp(const void* __lhs, const void* __rhs, size_t __count);
void* memchr(const void* __ptr, int __ch, size_t __count);
void* memmove(void* __dest, const void* __src, size_t __size);

int strcmp(const char* __lhs, const char* __rhs);
int strncmp(const char* __lhs, const char* __rhs, size_t __count);
char* strchr(const char* __str, int __ch);
char* strrchr(const char* __str, int __ch);
char* strstr(const char* __str, const char* __substr);
char* strpbrk(const char* __str, const char* __break_set);
char* strcpy(char* __restrict __dest, const char* __restrict __src);
char* strncpy(char* __restrict __dest, const char* __restrict __src, size_t __count);
char* strcat(char* __restrict __dest, const char* __restrict __src);
char* strncat(char* __restrict __dest, const char* __restrict __src, size_t __count);
char* strdup(const char* __str);
char* strndup(const char* __str, size_t __size);
size_t strspn(const char* __str, const char* __search);
size_t strcspn(const char* __str, const char* __search);
char* strtok(char* __restrict __str, const char* __delim);
int strcoll(const char* __lhs, const char* __rhs);
size_t strxfrm(char* __restrict __dest, const char* __restrict __src, size_t __count);

char* strerror(int __err_num);

// posix
size_t strnlen(const char* __str, size_t __max_len);
char* stpcpy(char* __restrict __dest, const char* __src);
char* strtok_r(char* __restrict __str, const char* __delim, char** __restrict __save_ptr);
int strcoll_l(const char* __lhs, const char* __rhs, locale_t __locale);
size_t strxfrm_l(char* __restrict __dest, const char* __restrict __src, size_t __count, locale_t __locale);

char* strerror_l(int __err_num, locale_t __locale);
char* strsignal(int __sig);

// glibc
void* rawmemchr(const void* __ptr, int __ch);
void* mempcpy(void* __restrict __dest, const void* __restrict __src, size_t __size);
void* memrchr(const void* __ptr, int __ch, size_t __count);
void* memmem(const void* __ptr, size_t __len, const void* __search, size_t __search_len);
char* strerror_r(int __err_num, char* __buf, size_t __buf_len);
int strverscmp(const char* __s1, const char* __s2);
char* strcasestr(const char* __str, const char* __substr);
char* strchrnul(const char* __str, int __ch);

__end_decls

#endif
