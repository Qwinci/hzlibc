#ifndef _CTYPE_H
#define _CTYPE_H

#include <bits/utils.h>
#include <stdint.h>

__begin

enum {
	_IsUpper = 1 << 8,
	_IsLower = 1 << 9,
	_IsAlpha = 1 << 10,
	_IsDigit = 1 << 11,
	_IsXdigit = 1 << 12,
	_IsSpace = 1 << 13,
	_IsPrint = 1 << 14,
	_IsGraph = 1 << 15,
	_IsBlank = 1 << 0,
	_IsCntrl = 1 << 1,
	_IsPunct = 1 << 2,
	_IsAlnum = 1 << 3
};

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

__end

#endif
