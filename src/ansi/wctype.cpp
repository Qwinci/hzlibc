#include "wctype.h"
#include "utils.hpp"
#include "ctype.h"

EXPORT int iswprint(wint_t ch) {
	if (ch >= 0x7F) {
		println("iswprint: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _IsPrint;
}

EXPORT int iswcntrl(wint_t ch) {
	if (ch >= 0x7F) {
		println("iswcntrl: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _IsCntrl;
}
