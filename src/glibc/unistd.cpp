#include "unistd.h"
#include "utils.hpp"
#include "fcntl.h"

EXPORT int euidaccess(const char* path, mode_t mode) {
	return faccessat(AT_FDCWD, path, static_cast<int>(mode), AT_EACCESS);
}

ALIAS(euidaccess, eaccess);
