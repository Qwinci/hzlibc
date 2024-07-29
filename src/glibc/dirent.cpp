#include "dirent.h"
#include "utils.hpp"
#include "string.h"

EXPORT int versionsort(const struct dirent** a, const struct dirent** b) {
	return strverscmp((*a)->d_name, (*b)->d_name);
}

EXPORT int versionsort64(const struct dirent64** a, const struct dirent64** b) {
	return strverscmp((*a)->d_name, (*b)->d_name);
}
