#include "nl_types.h"
#include "utils.hpp"

EXPORT nl_catd catopen(const char* __cat_name, int __flag) {
	println("catopen is a stub");
	return reinterpret_cast<nl_catd>(1);
}

EXPORT char* catgets(nl_catd catalog, int set, int number, const char* str) {
	println("catgets is a stub");
	return const_cast<char*>(str);
}

EXPORT int catclose(nl_catd catalog) {
	return 0;
}
