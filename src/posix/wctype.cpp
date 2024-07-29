#include "wctype.h"
#include "utils.hpp"

EXPORT wint_t towupper_l(wint_t ch, locale_t locale) {
	println("towupper_l ignores locale");
	return towupper(ch);
}

EXPORT wint_t towlower_l(wint_t ch, locale_t locale) {
	println("towlower_l ignores locale");
	return towlower(ch);
}

EXPORT wctype_t wctype_l(const char* property, locale_t locale) {
	println("wctype_l ignores locale");
	return wctype(property);
}

EXPORT int iswctype_l(wint_t wc, wctype_t desc, locale_t locale) {
	println("iswctype_l ignores locale");
	return iswctype(wc, desc);
}

ALIAS(towupper_l, __towupper_l);
ALIAS(towlower_l, __towlower_l);
ALIAS(wctype_l, __wctype_l);
ALIAS(iswctype_l, __iswctype_l);
