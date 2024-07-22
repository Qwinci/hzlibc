#include "wchar.h"
#include "utils.hpp"

EXPORT int mbsinit(const mbstate_t*) {
	return 1;
}
