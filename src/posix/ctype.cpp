#include "ctype.h"
#include "utils.hpp"

EXPORT int isascii(int ch) {
	return ch >= 0 && ch <= 0x7F;
}
