#include "langinfo.h"
#include "utils.hpp"

static constexpr hz::string_view MONTH_ABBREVS[] {
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

static constexpr hz::string_view MONTHS[] {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

EXPORT char* nl_langinfo(nl_item item) {
	int category = _NL_ITEM_CATEGORY(item);
	int index = _NL_ITEM_INDEX(item);

	if (category < 0 || category == LC_ALL || category >= 13) {
		return const_cast<char*>("");
	}

	if (category == LC_CTYPE) {
		if (index == _NL_ITEM_INDEX(_NL_CTYPE_CODESET_NAME)) {
			return const_cast<char*>("UTF-8");
		}
	}
	else if (category == LC_NUMERIC) {
		if (index == _NL_ITEM_INDEX(__DECIMAL_POINT)) {
			return const_cast<char*>(".");
		}
		else if (index == _NL_ITEM_INDEX(__THOUSANDS_SEP)) {
			return const_cast<char*>("");
		}
	}
	else if (category == LC_TIME) {
		if (index >= _NL_ITEM_INDEX(ABMON_1) && index <= _NL_ITEM_INDEX(ABMON_12)) {
			return const_cast<char*>(MONTH_ABBREVS[index - _NL_ITEM_INDEX(ABMON_1)].data());
		}
		else if (index >= _NL_ITEM_INDEX(MON_1) && index <= _NL_ITEM_INDEX(MON_12)) {
			return const_cast<char*>(MONTHS[index - _NL_ITEM_INDEX(MON_1)].data());
		}
	}

	println("nl_langinfo: category ", category, " index ", index, " is not implemented");
	__ensure(!"nl_langinfo is not implemented");
}

EXPORT char* nl_langinfo_l(nl_item item, locale_t locale) {
	//println("nl_langinfo_l ignores locale");
	return nl_langinfo(item);
}

ALIAS(nl_langinfo_l, __nl_langinfo_l);
