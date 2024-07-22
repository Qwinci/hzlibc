#include "arpa/inet.h"
#include "utils.hpp"
#include <hz/bit.hpp>

EXPORT uint32_t ntohl(uint32_t net_long) {
	return hz::to_ne_from_be(net_long);
}
