#ifndef _WCTYPE_H
#define _WCTYPE_H

#include <bits/utils.h>
#include <wchar.h>
#include <locale.h>

__begin

typedef unsigned long wctype_t;

int iswprint(wint_t __ch);
int iswcntrl(wint_t __ch);

wint_t towupper(wint_t __ch);
wint_t towlower(wint_t __ch);

wctype_t wctype(const char* __property);
int iswctype(wint_t __wc, wctype_t __desc);

// posix
wint_t towupper_l(wint_t __ch, locale_t __locale);
wint_t towlower_l(wint_t __ch, locale_t __locale);
wctype_t wctype_l(const char* __property, locale_t __locale);
int iswctype_l(wint_t __wc, wctype_t __desc, locale_t __locale);

__end

#endif
