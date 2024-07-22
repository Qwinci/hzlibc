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

	if (category == LC_TIME) {
		if (index >= _NL_ITEM_INDEX(ABMON_1) && index <= _NL_ITEM_INDEX(ABMON_12)) {
			return const_cast<char*>(MONTH_ABBREVS[index - _NL_ITEM_INDEX(ABMON_1)].data());
		}
		else if (index >= _NL_ITEM_INDEX(MON_1) && index <= _NL_ITEM_INDEX(MON_12)) {
			return const_cast<char*>(MONTHS[index - _NL_ITEM_INDEX(MON_1)].data());
		}
	}

	__ensure(!"nl_langinfo is not implemented");
}
