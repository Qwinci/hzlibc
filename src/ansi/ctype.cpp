#include "ctype.h"
#include "utils.hpp"

EXPORT int toupper(int ch) {
	return (*__ctype_toupper_loc())[ch];
}

EXPORT int tolower(int ch) {
	return (*__ctype_tolower_loc())[ch];
}

EXPORT int isupper(int ch) {
	return (*__ctype_b_loc())[ch] & _ISupper;
}

EXPORT int islower(int ch) {
	return (*__ctype_b_loc())[ch] & _ISlower;
}

EXPORT int isalpha(int ch) {
	return (*__ctype_b_loc())[ch] & _ISalpha;
}

EXPORT int isdigit(int ch) {
	return (*__ctype_b_loc())[ch] & _ISdigit;
}

EXPORT int isxdigit(int ch) {
	return (*__ctype_b_loc())[ch] & _ISxdigit;
}

EXPORT int isspace(int ch) {
	return (*__ctype_b_loc())[ch] & _ISspace;
}

EXPORT int isprint(int ch) {
	return (*__ctype_b_loc())[ch] & _ISprint;
}

EXPORT int isgraph(int ch) {
	return (*__ctype_b_loc())[ch] & _ISgraph;
}

EXPORT int isblank(int ch) {
	return (*__ctype_b_loc())[ch] & _ISblank;
}

EXPORT int iscntrl(int ch) {
	return (*__ctype_b_loc())[ch] & _IScntrl;
}

EXPORT int ispunct(int ch) {
	return (*__ctype_b_loc())[ch] & _ISpunct;
}

EXPORT int isalnum(int ch) {
	return (*__ctype_b_loc())[ch] & _ISalnum;
}
