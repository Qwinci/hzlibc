#include "ctype.h"
#include "utils.hpp"

EXPORT int toupper(int ch) {
	return (*__ctype_toupper_loc())[ch];
}

EXPORT int tolower(int ch) {
	return (*__ctype_tolower_loc())[ch];
}

EXPORT int isdigit(int ch) {
	return (*__ctype_b_loc())[ch] & _IsDigit;
}
