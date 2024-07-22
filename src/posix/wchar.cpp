#include "wchar.h"
#include "utils.hpp"

EXPORT int wcwidth(wchar_t ch) {
	if (ch >= 0x7F) {
		println("wcwidth: wide char support is not implemented");
	}
	return 1;
}
