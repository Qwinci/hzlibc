#include "arpa/inet.h"
#include "netinet/in.h"
#include "utils.hpp"
#include "errno.h"
#include "stdio.h"
#include "net_utils.hpp"
#include <hz/bit.hpp>

EXPORT uint16_t ntohs(uint16_t net_short) {
	return hz::to_ne_from_be(net_short);
}

EXPORT uint32_t ntohl(uint32_t net_long) {
	return hz::to_ne_from_be(net_long);
}

EXPORT const char* inet_ntop(int af, const void* __restrict src, char* dest, socklen_t size) {
	switch (af) {
		case AF_INET:
		{
			if (size < INET_ADDRSTRLEN) {
				errno = ENOSPC;
				return nullptr;
			}

			auto* addr = static_cast<const in_addr*>(src);
			auto value = addr->s_addr;
			__ensure(snprintf(
				dest,
				size,
				"%d.%d.%d.%d",
				value & 0xFF,
				value >> 8 & 0xFF,
				value >> 16 & 0xFF,
				value >> 24) != -1);
			return dest;
		}
		case AF_INET6:
		{
			if (size < INET6_ADDRSTRLEN) {
				errno = ENOSPC;
				return nullptr;
			}

			auto* addr = static_cast<const in6_addr*>(src);
			__ensure(snprintf(
				dest,
				size,
				"%x:%x:%x:%x:%x:%x:%x:%x",
				hz::to_ne_from_be(addr->s6_addr16[0]),
				hz::to_ne_from_be(addr->s6_addr16[1]),
				hz::to_ne_from_be(addr->s6_addr16[2]),
				hz::to_ne_from_be(addr->s6_addr16[3]),
				hz::to_ne_from_be(addr->s6_addr16[4]),
				hz::to_ne_from_be(addr->s6_addr16[5]),
				hz::to_ne_from_be(addr->s6_addr16[6]),
				hz::to_ne_from_be(addr->s6_addr16[7])) != -1);
			return dest;
		}
		default:
			errno = EAFNOSUPPORT;
			return nullptr;
	}
}

EXPORT int inet_pton(int af, const char* __restrict src, void* __restrict dest) {
	switch (af) {
		case AF_INET:
		{
			auto* addr = static_cast<in_addr*>(dest);
			if (auto ip = parse_ipv4(src)) {
				*addr = *ip;
				return 1;
			}
			else {
				return 0;
			}
		}
		case AF_INET6:
		{
			auto* addr = static_cast<in6_addr*>(dest);
			if (auto ip = parse_ipv6(src)) {
				*addr = *ip;
				return 1;
			}
			else {
				return 0;
			}
		}
		default:
			errno = EAFNOSUPPORT;
			return -1;
	}
}

EXPORT in6_addr in6addr_loopback IN6ADDR_LOOPBACK_INIT;
EXPORT in6_addr in6addr_any IN6ADDR_ANY_INIT;
