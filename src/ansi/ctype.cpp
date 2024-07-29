#include "ctype.h"
#include "utils.hpp"

EXPORT int toupper(int ch) {
	return (*__ctype_toupper_loc())[ch];
}

EXPORT int tolower(int ch) {
	return (*__ctype_tolower_loc())[ch];
}

EXPORT int isupper(int ch) {
	return (*__ctype_b_loc())[ch] & _IsUpper;
}

EXPORT int islower(int ch) {
	return (*__ctype_b_loc())[ch] & _IsLower;
}

EXPORT int isalpha(int ch) {
	return (*__ctype_b_loc())[ch] & _IsAlpha;
}

EXPORT int isdigit(int ch) {
	return (*__ctype_b_loc())[ch] & _IsDigit;
}

EXPORT int isxdigit(int ch) {
	return (*__ctype_b_loc())[ch] & _IsXdigit;
}

EXPORT int isspace(int ch) {
	return (*__ctype_b_loc())[ch] & _IsSpace;
}

EXPORT int isprint(int ch) {
	return (*__ctype_b_loc())[ch] & _IsPrint;
}

EXPORT int isgraph(int ch) {
	return (*__ctype_b_loc())[ch] & _IsGraph;
}

EXPORT int isblank(int ch) {
	return (*__ctype_b_loc())[ch] & _IsBlank;
}

EXPORT int iscntrl(int ch) {
	return (*__ctype_b_loc())[ch] & _IsCntrl;
}

EXPORT int ispunct(int ch) {
	return (*__ctype_b_loc())[ch] & _IsPunct;
}

EXPORT int isalnum(int ch) {
	return (*__ctype_b_loc())[ch] & _IsAlnum;
}
