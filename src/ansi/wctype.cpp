#include "wctype.h"
#include "utils.hpp"
#include "ctype.h"

EXPORT int iswupper(wint_t ch) {
	if (ch > 0x7F) {
		println("iswupper: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISupper;
}

EXPORT int iswlower(wint_t ch) {
	if (ch > 0x7F) {
		println("iswlower: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISlower;
}

EXPORT int iswalpha(wint_t ch) {
	if (ch > 0x7F) {
		println("iswalpha: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISalpha;
}

EXPORT int iswdigit(wint_t ch) {
	if (ch > 0x7F) {
		println("iswdigit: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISdigit;
}

EXPORT int iswxdigit(wint_t ch) {
	if (ch > 0x7F) {
		println("iswxdigit: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISxdigit;
}

EXPORT int iswspace(wint_t ch) {
	if (ch > 0x7F) {
		println("iswspace: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISspace;
}

EXPORT int iswprint(wint_t ch) {
	if (ch > 0x7F) {
		println("iswprint: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISprint;
}

EXPORT int iswgraph(wint_t ch) {
	if (ch > 0x7F) {
		println("iswgraph: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISgraph;
}

EXPORT int iswblank(wint_t ch) {
	if (ch > 0x7F) {
		println("iswblank: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISblank;
}

EXPORT int iswcntrl(wint_t ch) {
	if (ch > 0x7F) {
		println("iswcntrl: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _IScntrl;
}

EXPORT int iswpunct(wint_t ch) {
	if (ch > 0x7F) {
		println("iswpunct: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISpunct;
}

EXPORT int iswalnum(wint_t ch) {
	if (ch > 0x7F) {
		println("iswalnum: wide char support is not implemented");
		return 0;
	}
	return (*__ctype_b_loc())[ch] & _ISalnum;
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
		return _ISalnum;
	}
	else if (property_str == "alpha") {
		return _ISalpha;
	}
	else if (property_str == "blank") {
		return _ISblank;
	}
	else if (property_str == "cntrl") {
		return _IScntrl;
	}
	else if (property_str == "digit") {
		return _ISdigit;
	}
	else if (property_str == "graph") {
		return _ISgraph;
	}
	else if (property_str == "lower") {
		return _ISlower;
	}
	else if (property_str == "print") {
		return _ISprint;
	}
	else if (property_str == "space") {
		return _ISspace;
	}
	else if (property_str == "upper") {
		return _ISupper;
	}
	else if (property_str == "xdigit") {
		return _ISxdigit;
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
