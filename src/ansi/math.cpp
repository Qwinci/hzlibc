#include "math.h"
#include "utils.hpp"

#undef isinf
#undef isnan

extern "C" EXPORT int isinf(double x) {
	return fpclassify(x) == FP_INFINITE;
}

extern "C" EXPORT int isnan(double x) {
	return fpclassify(x) == FP_NAN;
}
