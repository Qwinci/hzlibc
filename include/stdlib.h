#ifndef _STDLIB_H
#define _STDLIB_H

#include "bits/utils.h"
#include "locale.h"
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

__begin

#define RAND_MAX 2147483647

void* malloc(size_t __size);
void* realloc(void* __old, size_t __new_size);
void* aligned_alloc(size_t __alignment, size_t __size);
void* calloc(size_t __num, size_t __size);
void free(void* __ptr);

char* getenv(const char* __name);

int system(const char* __cmd);

__attribute__((__noreturn__)) void exit(int __status);
__attribute__((__noreturn__)) void quick_exit(int __status);
__attribute__((__noreturn__)) void _Exit(int __status);
__attribute__((__noreturn__)) void abort(void);
int atexit(void (*__func)(void));

int atoi(const char* __str);
double atof(const char* __str);
long strtol(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);
unsigned long strtoul(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);
long long strtoll(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);
unsigned long long strtoull(const char* __restrict __ptr, char** __restrict __end_ptr, int __base);

float strtof(const char* __restrict __ptr, char** __restrict __end_ptr);
double strtod(const char* __restrict __ptr, char** __restrict __end_ptr);
long double strtold(const char* __restrict __ptr, char** __restrict __end_ptr);

void qsort(void* __ptr, size_t __count, size_t __size, int (*__comp)(const void* __a, const void* __b));

int mblen(const char* __str, size_t __len);
size_t mbstowcs(wchar_t* __restrict __dest, const char* __restrict __src, size_t __len);
int mbtowc(wchar_t* __restrict __pwc, const char* __restrict __str, size_t __len);
int wctomb(char* __str, wchar_t __wc);

void srand(unsigned int __seed);
int rand(void);

void* bsearch(
	const void* __key,
	const void* __ptr,
	size_t __count,
	size_t __size,
	int (*__comp)(const void* __a, const void* __b));

// posix
int posix_memalign(void** __ptr, size_t __alignment, size_t __size);

int setenv(const char* __name, const char* __value, int __overwrite);
int unsetenv(const char* __name);

char* realpath(const char* __restrict __path, char* __restrict __resolved_path);

float strtof_l(const char* __restrict __ptr, char** __restrict __end_ptr, locale_t __locale);
double strtod_l(const char* __restrict __ptr, char** __restrict __end_ptr, locale_t __locale);
long double strtold_l(const char* __restrict __ptr, char** __restrict __end_ptr, locale_t __locale);

int mkstemp(char* __template);

// glibc
void* reallocarray(void* __old, size_t __num_blocks, size_t __size);
char* secure_getenv(const char* __name);
int clearenv(void);

int mkostemp(char* __template, int __flags);

struct random_data {
	int32_t* fptr;
	int32_t* rptr;
	int32_t* state;
	int rand_type;
	int rand_deg;
	int rand_sep;
	int32_t* end_ptr;
};

int initstate_r(
	unsigned int __seed,
	char* __restrict __state_buf,
	size_t __state_len,
	struct random_data* __restrict __buf);
int random_r(struct random_data* __restrict __buf, int32_t* __restrict __result);

void qsort_r(
	void* __ptr,
	size_t __count,
	size_t __size,
	int (*__comp)(const void* __a, const void* __b, void* __arg),
	void* __arg);

int strfromf128(
	char* __restrict __buf,
	size_t __buf_size,
	const char* __restrict __fmt,
	__float128 __value);
__float128 strtof128(const char* __restrict __str, char** __restrict __end_ptr);

__end

#endif
