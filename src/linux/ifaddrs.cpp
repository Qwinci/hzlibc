#include "ifaddrs.h"
#include "utils.hpp"

EXPORT int getifaddrs(struct ifaddrs** ifap) {
	panic("getifaddrs is not implemented");
}

EXPORT void freeifaddrs(struct ifaddrs* ifa) {

}
