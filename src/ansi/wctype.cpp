#include "wctype.h"
#include "utils.hpp"
#include "ctype.h"

EXPORT int iswprint(wint_t ch) {
	if (ch > 0x7F) {
		println("iswprint: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _IsPrint;
}

EXPORT int iswcntrl(wint_t ch) {
	if (ch > 0x7F) {
		println("iswcntrl: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _IsCntrl;
}

EXPORT wint_t towupper(wint_t ch) {
	if (ch > 0x7F) {
		println("towupper: wide char support is not implemented");
		return 0;
	}
	return toupper(static_cast<int>(ch));
}

EXPORT wint_t towlower(wint_t ch) {
	if (ch > 0x7F) {
		println("towlower: wide char support is not implemented");
		return 0;
	}
	return tolower(static_cast<int>(ch));
}

EXPORT wctype_t wctype(const char* property) {
	hz::string_view property_str {property};
	if (property_str == "alnum") {
		return _IsAlnum;
	}
	else if (property_str == "alpha") {
		return _IsAlpha;
	}
	else if (property_str == "blank") {
		return _IsBlank;
	}
	else if (property_str == "cntrl") {
		return _IsCntrl;
	}
	else if (property_str == "digit") {
		return _IsDigit;
	}
	else if (property_str == "graph") {
		return _IsGraph;
	}
	else if (property_str == "lower") {
		return _IsLower;
	}
	else if (property_str == "print") {
		return _IsPrint;
	}
	else if (property_str == "space") {
		return _IsSpace;
	}
	else if (property_str == "upper") {
		return _IsUpper;
	}
	else if (property_str == "xdigit") {
		return _IsXdigit;
	}
	else {
		return 0;
	}
}

EXPORT int iswctype(wint_t wc, wctype_t desc) {
	if (wc > 0x7F) {
		println("iswctype: wide char support is not implemented");
		return 0;
	}
	return !!((*__ctype_b_loc())[wc] & desc);
}
