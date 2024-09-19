#include "arpa/inet.h"
#include "netinet/in.h"
#include "utils.hpp"
#include "errno.h"
#include "stdio.h"
#include "net_utils.hpp"
#include <hz/bit.hpp>

EXPORT uint16_t htons(uint16_t host_short) {
	return hz::to_be(host_short);
}

EXPORT uint32_t htonl(uint32_t host_long) {
	return hz::to_be(host_long);
}

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

			int zero_count = 0;
			for (int i = 0; i < 8; ++i) {
				if (!addr->s6_addr16[i]) {
					++zero_count;
				}
				else {
					break;
				}
			}

			if (zero_count) {
				snprintf(dest, size, "::");
				dest += 2;
				size -= 2;
			}

			for (int i = 0; i < 8 - zero_count; ++i) {
				if (i == 0) {
					int count = snprintf(dest, size, "%x", hz::to_ne_from_be(addr->s6_addr16[zero_count + i]));
					dest += count;
					size -= count;
				}
				else {
					int count = snprintf(dest, size, ":%x", hz::to_ne_from_be(addr->s6_addr16[zero_count + i]));
					dest += count;
					size -= count;
				}
			}

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

EXPORT in_addr_t inet_addr(const char* str) {
	if (auto ip = parse_ipv4(str)) {
		return ip->s_addr;
	}
	else {
		return -1;
	}
}

namespace {
	char INET_NTOA_BUF[INET_ADDRSTRLEN];
}

EXPORT char* inet_ntoa(struct in_addr addr) {
	__ensure(inet_ntop(AF_INET, &addr, INET_NTOA_BUF, sizeof(INET_NTOA_BUF)));
	return INET_NTOA_BUF;
}

EXPORT in6_addr in6addr_loopback IN6ADDR_LOOPBACK_INIT;
EXPORT in6_addr in6addr_any IN6ADDR_ANY_INIT;
