#pragma once
#include "log.hpp"

#define EXPORT [[gnu::visibility("default")]]
#define ALIAS(old, name) extern "C" EXPORT [[gnu::alias(#old)]] typeof(old) name
#define ALIAS_VAR(old, name) extern "C" EXPORT [[gnu::alias(#old)]] typeof(old) name

#define __ensure(expr) ((expr) ? (void) 0 : panic(__FILE__, ":", __LINE__, " (", __func__, "): assertion '", #expr, "' failed"))
