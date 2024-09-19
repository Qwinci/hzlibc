#include "syslog.h"
#include "utils.hpp"

namespace {
	int MASK = 0;
}

EXPORT int setlogmask(int mask) {
	int prev = MASK;
	MASK = mask;
	println("setlogmask is ignored");
	return prev;
}
