#ifndef _WCTYPE_H
#define _WCTYPE_H

#include <bits/utils.h>
#include <bits/config.h>
#include <wchar.h>

#if !__HZLIBC_ANSI_ONLY
#include <locale.h>
#endif

__begin_decls

typedef unsigned long wctype_t;

int iswupper(wint_t __ch);
int iswlower(wint_t __ch);
int iswalpha(wint_t __ch);
int iswdigit(wint_t __ch);
int iswxdigit(wint_t __ch);
int iswspace(wint_t __ch);
int iswprint(wint_t __ch);
int iswgraph(wint_t __ch);
int iswblank(wint_t __ch);
int iswcntrl(wint_t __ch);
int iswpunct(wint_t __ch);
int iswalnum(wint_t __ch);

wint_t towupper(wint_t __ch);
wint_t towlower(wint_t __ch);

wctype_t wctype(const char* __property);
int iswctype(wint_t __wc, wctype_t __desc);

#if !__HZLIBC_ANSI_ONLY

// posix
wint_t towupper_l(wint_t __ch, locale_t __locale);
wint_t towlower_l(wint_t __ch, locale_t __locale);
wctype_t wctype_l(const char* __property, locale_t __locale);
int iswctype_l(wint_t __wc, wctype_t __desc, locale_t __locale);

#endif

__end_decls

#endif
