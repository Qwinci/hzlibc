#ifndef _CTYPE_H
#define _CTYPE_H

#include <bits/utils.h>
#include <bits/config.h>
#include <stdint.h>
#include <locale.h>

__begin_decls

enum {
	_ISupper = 1 << 8,
	_ISlower = 1 << 9,
	_ISalpha = 1 << 10,
	_ISdigit = 1 << 11,
	_ISxdigit = 1 << 12,
	_ISspace = 1 << 13,
	_ISprint = 1 << 14,
	_ISgraph = 1 << 15,
	_ISblank = 1 << 0,
	_IScntrl = 1 << 1,
	_ISpunct = 1 << 2,
	_ISalnum = 1 << 3
};

#define _ISbit(bit) ((bit) < 8 ? ((1 << ((bit) + 8))) : ((1 << ((bit) - 8))))

extern const unsigned short** __ctype_b_loc(void);
extern const int32_t** __ctype_tolower_loc(void);
extern const int32_t** __ctype_toupper_loc(void);

int toupper(int __ch);
int tolower(int __ch);

int isupper(int __ch);
int islower(int __ch);
int isalpha(int __ch);
int isdigit(int __ch);
int isxdigit(int __ch);
int isspace(int __ch);
int isprint(int __ch);
int isgraph(int __ch);
int isblank(int __ch);
int iscntrl(int __ch);
int ispunct(int __ch);
int isalnum(int __ch);

#if !__HZLIBC_ANSI_ONLY

// posix
int isascii(int __ch);

int toupper_l(int __ch, locale_t __locale);
int tolower_l(int __ch, locale_t __locale);

int isupper_l(int __ch, locale_t __locale);
int islower_l(int __ch, locale_t __locale);
int isalpha_l(int __ch, locale_t __locale);
int isdigit_l(int __ch, locale_t __locale);
int isxdigit_l(int __ch, locale_t __locale);
int isspace_l(int __ch, locale_t __locale);
int isprint_l(int __ch, locale_t __locale);
int isgraph_l(int __ch, locale_t __locale);
int isblank_l(int __ch, locale_t __locale);
int iscntrl_l(int __ch, locale_t __locale);
int ispunct_l(int __ch, locale_t __locale);
int isalnum_l(int __ch, locale_t __locale);

#endif

__end_decls

#endif
