#include "locale.h"
#include "utils.hpp"
#include "ctype.h"

namespace {
	__locale C_LOCALE {
		.__locales {},
		.__ctype_b = *__ctype_b_loc(),
		.__ctype_tolower = *__ctype_tolower_loc(),
		.__ctype_toupper = *__ctype_toupper_loc(),
		.__names {}
	};

	thread_local locale_t CURRENT_LOCALE = &C_LOCALE;
	locale_t GLOBAL_LOCALE = &C_LOCALE;
}

EXPORT locale_t newlocale(int category, const char* locale, locale_t base) {
	return &C_LOCALE;
}

EXPORT void freelocale(locale_t locale) {

}

EXPORT locale_t duplocale(locale_t locale) {
	return &C_LOCALE;
}

EXPORT locale_t uselocale(locale_t new_locale) {
	if (!new_locale) {
		return CURRENT_LOCALE;
	}
	else if (new_locale == LC_GLOBAL_LOCALE) {
		new_locale = GLOBAL_LOCALE;
	}

	auto old = CURRENT_LOCALE;
	CURRENT_LOCALE = new_locale;
	return old;
}

ALIAS(newlocale, __newlocale);
ALIAS(freelocale, __freelocale);
ALIAS(duplocale, __duplocale);
ALIAS(uselocale, __uselocale);
