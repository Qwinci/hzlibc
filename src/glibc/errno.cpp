#include "errno.h"
#include "utils.hpp"

EXPORT char* __progname = const_cast<char*>("");
EXPORT char* __progname_full = const_cast<char*>("");

ALIAS_VAR(__progname, program_invocation_short_name);
ALIAS_VAR(__progname_full, program_invocation_name);
