#include "netdb.h"
#include "utils.hpp"
#include "allocator.hpp"

EXPORT int gethostbyname_r(const char* name, hostent* ret, char* buf, size_t buf_len, hostent** res, int* h_errnop) {
	__ensure(!"gethostbyname_r is not implemented");
}
