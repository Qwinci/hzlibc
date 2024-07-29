#ifndef _LOCALE_H
#define _LOCALE_H

#include <bits/utils.h>

__begin

struct lconv {
	char* decimal_point;
	char* thousands_sep;
	char* grouping;
	char* int_curr_symbol;
	char* currency_symbol;
	char* mon_decimal_point;
	char* mon_thousands_sep;
	char* mon_grouping;
	char* positive_sign;
	char* negative_sign;
	char int_frac_digits;
	char frac_digits;
	char p_cs_precedes;
	char p_sep_by_space;
	char n_cs_precedes;
	char n_sep_by_space;
	char p_sign_posn;
	char n_sign_posn;
	char int_p_cs_precedes;
	char int_p_sep_by_space;
	char int_n_cs_precedes;
	char int_n_sep_by_space;
	char int_p_sign_posn;
	char int_n_sign_posn;
};

#define LC_CTYPE 0
#define LC_NUMERIC 1
#define LC_TIME 2
#define LC_COLLATE 3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL 6
#define LC_PAPER 7
#define LC_NAME 8
#define LC_ADDRESS 9
#define LC_TELEPHONE 10
#define LC_MEASUREMENT 11
#define LC_IDENTIFICATION 12

#define LC_GLOBAL_LOCALE ((locale_t) -1L)

typedef struct __locale {
	struct __locale_data* __locales[13];
	const unsigned short* __ctype_b;
	const int* __ctype_tolower;
	const int* __ctype_toupper;
	const char* __names[13];
} *locale_t;

char* setlocale(int __category, const char* __locale);
struct lconv* localeconv(void);

// posix
locale_t newlocale(int __category, const char* __locale, locale_t __base);
void freelocale(locale_t __locale);
locale_t duplocale(locale_t __locale);
locale_t uselocale(locale_t __new_locale);

__end

#endif
