#include "utils.hpp"

extern "C" EXPORT void __stack_chk_fail() {
	panic("stack smashing detected");
}
