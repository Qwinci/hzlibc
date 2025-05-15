#include "wctype.h"
#include "utils.hpp"

EXPORT wint_t towupper_l(wint_t ch, locale_t locale) {
	//println("towupper_l ignores locale");
	return towupper(ch);
}

EXPORT wint_t towlower_l(wint_t ch, locale_t locale) {
	//println("towlower_l ignores locale");
	return towlower(ch);
}

EXPORT int iswupper_l(wint_t ch, locale_t __locale) {
	//println("iswupper_l ignores locale");
	return iswupper(ch);
}

EXPORT int iswlower_l(wint_t ch, locale_t __locale) {
	//println("iswlower_l ignores locale");
	return iswlower(ch);
}

EXPORT int iswalpha_l(wint_t ch, locale_t __locale) {
	//println("iswalpha_l ignores locale");
	return iswalpha(ch);
}

EXPORT int iswdigit_l(wint_t ch, locale_t __locale) {
	//println("iswdigit_l ignores locale");
	return iswdigit(ch);
}

EXPORT int iswxdigit_l(wint_t ch, locale_t __locale) {
	//println("iswxdigit_l ignores locale");
	return iswxdigit(ch);
}

EXPORT int iswspace_l(wint_t ch, locale_t __locale) {
	//println("iswspace_l ignores locale");
	return iswspace(ch);
}

EXPORT int iswprint_l(wint_t ch, locale_t __locale) {
	//println("iswprint_l ignores locale");
	return iswprint(ch);
}

EXPORT int iswgraph_l(wint_t ch, locale_t __locale) {
	//println("iswgraph_l ignores locale");
	return iswgraph(ch);
}

EXPORT int iswblank_l(wint_t ch, locale_t __locale) {
	//println("iswblank_l ignores locale");
	return iswblank(ch);
}

EXPORT int iswcntrl_l(wint_t ch, locale_t __locale) {
	//println("iswcntrl_l ignores locale");
	return iswcntrl(ch);
}

EXPORT int iswpunct_l(wint_t ch, locale_t __locale) {
	//println("iswpunct_l ignores locale");
	return iswpunct(ch);
}

EXPORT int iswalnum_l(wint_t ch, locale_t __locale) {
	//println("iswalnum_l ignores locale");
	return iswalnum(ch);
}

EXPORT wctype_t wctype_l(const char* property, locale_t locale) {
	//println("wctype_l ignores locale");
	return wctype(property);
}

EXPORT int iswctype_l(wint_t wc, wctype_t desc, locale_t locale) {
	//println("iswctype_l ignores locale");
	return iswctype(wc, desc);
}

ALIAS(towupper_l, __towupper_l);
ALIAS(towlower_l, __towlower_l);
ALIAS(wctype_l, __wctype_l);
ALIAS(iswctype_l, __iswctype_l);
