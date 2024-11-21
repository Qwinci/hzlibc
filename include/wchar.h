#ifndef _WCHAR_H
#define _WCHAR_H

#include <bits/utils.h>
#include <bits/config.h>
#include <bits/mbstate_t.h>
#include <stddef.h>
#include <stdarg.h>
#include <time.h>

#if !__HZLIBC_ANSI_ONLY
#include <locale.h>
#endif

__begin_decls

typedef __WINT_TYPE__ wint_t;

#if !defined(__cplusplus) && !defined(_STDLIB_H)
typedef __WCHAR_TYPE__ wchar_t;
#endif

typedef struct FILE FILE;

#define WEOF 0xFFFFFFFF

int mbsinit(const mbstate_t* __ps);
size_t mbrlen(const char* __restrict __str, size_t __len, mbstate_t* __restrict __ps);
size_t wcrtomb(char* __restrict __str, wchar_t __wc, mbstate_t* __restrict __ps);
size_t mbrtowc(
	wchar_t* __restrict __pwc,
	const char* __restrict __str,
	size_t __len,
	mbstate_t* __restrict __ps);
size_t mbsrtowcs(
	wchar_t* __restrict __dest,
	const char** __restrict __src,
	size_t __len,
	mbstate_t* __restrict __ps);

int wctob(wint_t __ch);
wint_t btowc(int __ch);
size_t wcsrtombs(char* __restrict __dest, const wchar_t** __restrict __src, size_t __len, mbstate_t* __restrict __ps);

size_t wcslen(const wchar_t* __str);
wchar_t* wcscat(wchar_t* __restrict __dest, const wchar_t* __restrict __src);
wchar_t* wcscpy(wchar_t* __restrict __dest, const wchar_t* __restrict __src);
wchar_t* wcsncpy(wchar_t* __restrict __dest, const wchar_t* __restrict __src, size_t __count);
int wcscmp(const wchar_t* __lhs, const wchar_t* __rhs);
int wcsncmp(const wchar_t* __lhs, const wchar_t* __rhs, size_t __count);
wchar_t* wcsrchr(const wchar_t* __str, wchar_t __ch);
wchar_t* wmemchr(const wchar_t* __ptr, wchar_t __ch, size_t __count);
wchar_t* wmemcpy(wchar_t* __restrict __dest, const wchar_t* __restrict __src, size_t __count);
wchar_t* wmemmove(wchar_t* __dest, const wchar_t* __src, size_t __count);
wchar_t* wmemset(wchar_t* __dest, wchar_t __ch, size_t __count);
int wmemcmp(const wchar_t* __lhs, const wchar_t* __rhs, size_t __count);
int wcscoll(const wchar_t* __lhs, const wchar_t* __rhs);
size_t wcsxfrm(wchar_t* __restrict __dest, const wchar_t* __restrict __src, size_t __count);
wchar_t* wcschr(const wchar_t* __str, wchar_t __ch);
wchar_t* wcsstr(const wchar_t* __str, const wchar_t* __substr);
wchar_t* wcstok(wchar_t* __restrict __str, const wchar_t* __restrict __delim, wchar_t** __restrict __save_ptr);
wchar_t* wcspbrk(const wchar_t* __str, const wchar_t* __break_set);

long wcstol(const wchar_t* __restrict __str, wchar_t** __restrict __end, int __base);
long long wcstoll(const wchar_t* __restrict __str, wchar_t** __restrict __end, int __base);
unsigned long wcstoul(const wchar_t* __restrict __str, wchar_t** __restrict __end, int __base);
unsigned long long wcstoull(const wchar_t* __restrict __str, wchar_t** __restrict __end, int __base);

float wcstof(const wchar_t* __restrict __str, wchar_t** __restrict __end);
double wcstod(const wchar_t* __restrict __str, wchar_t** __restrict __end);
long double wcstold(const wchar_t* __restrict __str, wchar_t** __restrict __end);

size_t wcsftime(
	wchar_t* __restrict __str,
	size_t __count,
	const wchar_t* __restrict __fmt,
	const struct tm* __time);

wint_t fgetwc(FILE* __file);
wint_t getwc(FILE* __file);
wint_t fputwc(wint_t __ch, FILE* __file);
wint_t putwc(wint_t __ch, FILE* __file);
wint_t putwchar(wchar_t __ch);
wint_t ungetwc(wint_t __ch, FILE* __file);
int fputws(const wchar_t* __restrict __str, FILE* __restrict __file);

int vswprintf(
	wchar_t* __restrict __buffer,
	size_t __size,
	const wchar_t* __restrict __fmt,
	va_list __ap);
int swprintf(
	wchar_t* __restrict __buffer,
	size_t __size,
	const wchar_t* __restrict __fmt,
	...);

#if !__HZLIBC_ANSI_ONLY

// posix
size_t wcsnlen(const wchar_t* __str, size_t __max_len);
int wcscasecmp(const wchar_t* __s1, const wchar_t* __s2);
int wcwidth(wchar_t __ch);
int wcswidth(const wchar_t* __str, size_t __len);
int wcscoll_l(const wchar_t* __lhs, const wchar_t* __rhs, locale_t __locale);
wchar_t* wcsdup(const wchar_t* __str);
size_t wcsxfrm_l(
	wchar_t* __restrict __dest,
	const wchar_t* __restrict __src,
	size_t __count,
	locale_t __locale);
size_t wcsftime_l(
	wchar_t* __restrict __str,
	size_t __count,
	const wchar_t* __restrict __fmt,
	const struct tm* __time,
	locale_t __locale);
size_t wcsnrtombs(
	char* __restrict __dest,
	const wchar_t** __restrict __src,
	size_t __num_wc,
	size_t __len,
	mbstate_t* __restrict __ps);
size_t mbsnrtowcs(
	wchar_t* __restrict __dest,
	const char** __restrict __src,
	size_t __num_chars,
	size_t __len,
	mbstate_t* __restrict __ps);

#endif

__end_decls

#endif
