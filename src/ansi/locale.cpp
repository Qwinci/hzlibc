#include "locale.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "limits.h"
#include <hz/string.hpp>

namespace {
	[[gnu::init_priority(101)]] hz::string<Allocator> LOCALE {Allocator {}};

	[[gnu::constructor(102)]] void init_locale() {
		LOCALE = "C";
	}

	constexpr char* s(const char* s) {
		return const_cast<char*>(s);
	}

	constexpr lconv C_LOCALE {
		.decimal_point = s("."),
		.thousands_sep = s(""),
		.grouping = s(""),
		.int_curr_symbol = s(""),
		.currency_symbol = s(""),
		.mon_decimal_point = s(""),
		.mon_thousands_sep = s(""),
		.mon_grouping = s(""),
		.positive_sign = s(""),
		.negative_sign = s(""),
		.int_frac_digits = CHAR_MAX,
		.frac_digits = CHAR_MAX,
		.p_cs_precedes = CHAR_MAX,
		.p_sep_by_space = CHAR_MAX,
		.n_cs_precedes = CHAR_MAX,
		.n_sep_by_space = CHAR_MAX,
		.p_sign_posn = CHAR_MAX,
		.n_sign_posn = CHAR_MAX,
		.int_p_cs_precedes = CHAR_MAX,
		.int_p_sep_by_space = CHAR_MAX,
		.int_n_cs_precedes = CHAR_MAX,
		.int_n_sep_by_space = CHAR_MAX,
		.int_p_sign_posn = CHAR_MAX,
		.int_n_sign_posn = CHAR_MAX
	};
}

EXPORT char* setlocale(int category, const char* locale) {
	if (!locale) {
		return LOCALE.data();
	}
	else if (!*locale) {
		LOCALE = "C";
		return LOCALE.data();
	}
	else {
		LOCALE = locale;
		return LOCALE.data();
	}
}

EXPORT lconv* localeconv() {
	return const_cast<lconv*>(&C_LOCALE);
}
