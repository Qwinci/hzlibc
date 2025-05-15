#include "ctype.h"
#include "utils.hpp"

EXPORT int isascii(int ch) {
	return ch >= 0 && ch <= 0x7F;
}

EXPORT int toupper_l(int ch, locale_t __locale) {
	//println("toupper_l ignores locale");
	return toupper(ch);
}

EXPORT int tolower_l(int ch, locale_t __locale) {
	//println("tolower_l ignores locale");
	return tolower(ch);
}

EXPORT int isupper_l(int ch, locale_t __locale) {
	//println("isupper_l ignores locale");
	return isupper(ch);
}

EXPORT int islower_l(int ch, locale_t __locale) {
	//println("islower_l ignores locale");
	return islower(ch);
}

EXPORT int isalpha_l(int ch, locale_t __locale) {
	//println("isalpha_l ignores locale");
	return isalpha(ch);
}

EXPORT int isdigit_l(int ch, locale_t __locale) {
	//println("isdigit_l ignores locale");
	return isdigit(ch);
}

EXPORT int isxdigit_l(int ch, locale_t __locale) {
	//println("isxdigit_l ignores locale");
	return isxdigit(ch);
}

EXPORT int isspace_l(int ch, locale_t __locale) {
	//println("isspace_l ignores locale");
	return isspace(ch);
}

EXPORT int isprint_l(int ch, locale_t __locale) {
	//println("isprint_l ignores locale");
	return isprint(ch);
}

EXPORT int isgraph_l(int ch, locale_t __locale) {
	//println("isgraph_l ignores locale");
	return isgraph(ch);
}

EXPORT int isblank_l(int ch, locale_t __locale) {
	//println("isblank_l ignores locale");
	return isblank(ch);
}

EXPORT int iscntrl_l(int ch, locale_t __locale) {
	//println("iscntrl_l ignores locale");
	return iscntrl(ch);
}

EXPORT int ispunct_l(int ch, locale_t __locale) {
	//println("ispunct_l ignores locale");
	return ispunct(ch);
}

EXPORT int isalnum_l(int ch, locale_t __locale) {
	//println("isalnum_l ignores locale");
	return isalnum(ch);
}

