#include "netdb.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "stdio.h"
#include "ctype.h"
#include "string.h"
#include "net_utils.hpp"
#include "errno.h"
#include "internal/dns.hpp"
#include <hz/string.hpp>
#include <hz/string_utils.hpp>
#include <hz/optional.hpp>
#include <hz/vector.hpp>
#include <hz/variant.hpp>
#include <hz/pair.hpp>

#define memcpy __builtin_memcpy

namespace {
	struct NameResolveResult {
		hz::vector<hz::variant<in_addr, in6_addr>, Allocator> ips {Allocator {}};
	};

	void resolve_name_in_hosts(hz::string_view name, bool ipv6, NameResolveResult& result) {
		FILE* file = fopen("/etc/hosts", "r");
		if (!file) {
			return;
		}

		char line_buf[256];
		while (fgets(line_buf, 256, file)) {
			hz::string_view line {line_buf};
			if (!line.size() || line.starts_with('#')) {
				continue;
			}
			if (auto comment_start = line.find('#'); comment_start != hz::string_view::npos) {
				line = line.substr_abs(0, comment_start);
			}

			auto ip_end = line.find([](char c) {
				return isspace(c);
			});
			if (ip_end == hz::string_view::npos || ip_end == 0) {
				continue;
			}

			auto ip = line.substr_abs(0, ip_end);

			auto offset = ip_end;
			while (true) {
				auto host_start = line.find([](char c) {
					return !isspace(c);
				}, offset);
				if (host_start == hz::string_view::npos) {
					break;
				}
				auto host_end = line.find([](char c) {
					return isspace(c);
				}, host_start);

				auto host = line.substr_abs(host_start, host_end);
				if (host == name) {
					if (ipv6) {
						if (auto addr = parse_ipv6(ip)) {
							result.ips.push_back(*addr);
						}
					}
					else {
						if (auto ipv4 = parse_ipv4(ip)) {
							result.ips.push_back(*ipv4);
						}
					}
				}

				if (host_end == hz::string_view::npos) {
					break;
				}
				else {
					offset = host_end;
				}
			}
		}

		fclose(file);
	}

	hz::optional<hz::string<Allocator>> resolve_addr_in_hosts(const void* ip, bool ipv6) {
		FILE* file = fopen("/etc/hosts", "r");
		if (!file) {
			return hz::nullopt;
		}

		char line_buf[256];
		while (fgets(line_buf, 256, file)) {
			hz::string_view line {line_buf};
			if (!line.size() || line.starts_with('#')) {
				continue;
			}
			if (auto comment_start = line.find('#'); comment_start != hz::string_view::npos) {
				line = line.substr_abs(0, comment_start);
			}

			auto ip_end = line.find([](char c) {
				return isspace(c);
			});
			if (ip_end == hz::string_view::npos || ip_end == 0) {
				continue;
			}

			auto ip_str = line.substr_abs(0, ip_end);
			if (ipv6) {
				if (auto addr = parse_ipv6(ip_str)) {
					if (memcmp(ip, &addr.value(), sizeof(in6_addr)) != 0) {
						continue;
					}
				}
				else {
					continue;
				}
			}
			else {
				if (auto addr = parse_ipv4(ip_str)) {
					if (memcmp(ip, &addr.value(), sizeof(in_addr)) != 0) {
						continue;
					}
				}
				else {
					continue;
				}
			}

			auto offset = ip_end;
			while (true) {
				auto host_start = line.find([](char c) {
					return !isspace(c);
				}, offset);
				if (host_start == hz::string_view::npos) {
					break;
				}
				auto host_end = line.find([](char c) {
					return isspace(c);
				}, host_start);

				auto host = line.substr_abs(host_start, host_end);
				hz::string<Allocator> res {Allocator {}};
				res = host;
				fclose(file);
				return res;
			}
		}

		fclose(file);
		return hz::nullopt;
	}

	hz::optional<hz::pair<uint16_t, bool>> resolve_name_in_services(hz::string_view name, bool tcp) {
		FILE* file = fopen("/etc/services", "r");
		if (!file) {
			return hz::nullopt;
		}

		char line_buf[256];
		while (fgets(line_buf, 256, file)) {
			hz::string_view line {line_buf};
			if (auto comment_start = line.find('#'); comment_start != hz::string_view::npos) {
				line = line.substr_abs(0, comment_start);
			}

			auto name_end = line.find([](char c) {
				return isspace(c);
			});

			auto service_name = line.substr_abs(0, name_end);

			auto port_start = line.find([](char c) {
				return !isspace(c);
			}, name_end);
			if (port_start == hz::string_view::npos) {
				continue;
			}
			auto port_end = line.find([](char c) {
				return isspace(c);
			}, port_start);
			auto port = line.substr_abs(port_start, port_end);

			if (service_name == name) {
				size_t count;
				auto real_port = hz::to_integer<uint16_t>(port, 10, &count);
				auto end = port.substr(count + 1);
				if ((end == "/tcp" && tcp) || (end == "/udp" && !tcp)) {
					fclose(file);
					return hz::pair {real_port, end == "/tcp"};
				}
			}

			size_t offset = port_end;
			while (true) {
				auto alias_start = line.find([](char c) {
					return !isspace(c);
				}, offset);
				auto alias_end = line.find([](char c) {
					return isspace(c);
				}, alias_start);

				auto alias = line.substr_abs(alias_start, alias_end);
				if (alias.size() == 0) {
					break;
				}

				if (alias == name) {
					size_t count;
					auto real_port = hz::to_integer<uint16_t>(port, 10, &count);
					auto end = port.substr(count);
					if ((end == "/tcp" && tcp) || (end == "/udp" && !tcp)) {
						fclose(file);
						return hz::pair {real_port, end == "/tcp"};
					}
				}

				if (alias_end == hz::string_view::npos) {
					break;
				}
				else {
					offset = alias_end + 1;
				}
			}
		}

		fclose(file);
		return hz::nullopt;
	}

	hz::optional<hz::string<Allocator>> resolve_port_in_services(uint16_t port) {
		FILE* file = fopen("/etc/services", "r");
		if (!file) {
			return hz::nullopt;
		}

		char line_buf[256];
		while (fgets(line_buf, 256, file)) {
			hz::string_view line {line_buf};
			if (auto comment_start = line.find('#'); comment_start != hz::string_view::npos) {
				line = line.substr_abs(0, comment_start);
			}

			auto name_end = line.find([](char c) {
				return isspace(c);
			});

			auto service_name = line.substr_abs(0, name_end);

			auto port_start = line.find([](char c) {
				return !isspace(c);
			}, name_end);
			if (port_start == hz::string_view::npos) {
				continue;
			}
			auto port_end = line.find([](char c) {
				return isspace(c);
			}, port_start);
			auto port_str = line.substr_abs(port_start, port_end);

			size_t count;
			auto real_port = hz::to_integer<uint16_t>(port_str, 10, &count);
			if (real_port == port) {
				hz::string<Allocator> res {Allocator {}};
				res = service_name;
				return {std::move(res)};
			}
		}

		fclose(file);
		return hz::nullopt;
	}

	constexpr const char* HEX_CHARS = "0123456789abcdef";

	int resolve_addr_dns(hz::string_view addr, bool ipv6, char* dest, size_t dest_size) {
		dns::Query query {dns::qtype::PTR};

		if (ipv6) {
			auto parsed = parse_ipv6(addr);
			__ensure(parsed);

			for (int i = 16; i > 0; --i) {
				auto byte = parsed.value().s6_addr[i - 1];
				query.add_segment(hz::string_view {&HEX_CHARS[byte & 0xF], 1});
				query.add_segment(hz::string_view {&HEX_CHARS[byte >> 4], 1});
			}

			query.add_segment("ip6");
		}
		else {
			size_t offset = 0;
			while (true) {
				auto next_offset = addr.find('.', offset);
				auto chunk = addr.substr_abs(offset, next_offset);
				query.add_segment(chunk);
				if (next_offset == hz::string_view::npos) {
					break;
				}
				else {
					offset = next_offset + 1;
				}
			}

			query.add_segment("in-addr");
		}

		query.add_segment("arpa");

		int err = query.send([&](uint16_t type, dns::Ptr ptr) {
			if (type == dns::qtype::PTR) {
				auto name = ptr.parse_dns_name();
				if (dest_size < name.size() + 1) {
					return EAI_OVERFLOW;
				}
				memcpy(dest, name.data(), name.size() + 1);
				return ESRCH;
			}
			else {
				return 0;
			}
		});

		switch (err) {
			case 0:
				return EAI_NONAME;
			case ESRCH:
				return 0;
			case EAI_OVERFLOW:
				return EAI_OVERFLOW;
			default:
				return EAI_FAIL;
		}
	}

	int resolve_canon_dns(hz::string_view name, hz::string<Allocator>& dest) {
		dns::Query query {dns::qtype::CNAME};

		size_t offset = 0;
		while (true) {
			auto next_offset = name.find('.', offset);
			auto chunk = name.substr_abs(offset, next_offset);
			query.add_segment(chunk);
			if (next_offset == hz::string_view::npos) {
				break;
			}
			else {
				offset = next_offset + 1;
			}
		}

		int err = query.send([&](uint16_t type, dns::Ptr ptr) {
			if (type == dns::qtype::CNAME) {
				dest = ptr.parse_dns_name();
				return ESRCH;
			}
			else {
				return 0;
			}
		});

		switch (err) {
			case 0:
				return EAI_NONAME;
			case ESRCH:
				return 0;
			default:
				return EAI_FAIL;
		}
	}

	int resolve_name_dns(hz::string_view name, bool ipv6, NameResolveResult& result) {
		dns::Query query {ipv6 ? dns::qtype::AAAA : dns::qtype::A};

		size_t offset = 0;
		while (true) {
			auto next_offset = name.find('.', offset);
			auto chunk = name.substr_abs(offset, next_offset);
			query.add_segment(chunk);
			if (next_offset == hz::string_view::npos) {
				break;
			}
			else {
				offset = next_offset + 1;
			}
		}

		int err = query.send([&](uint16_t type, dns::Ptr ptr) {
			if (ipv6 && type == dns::qtype::AAAA) {
				result.ips.push_back(ptr.parse_ipv6());
			}
			else if (!ipv6 && type == dns::qtype::A) {
				result.ips.push_back(ptr.parse_ipv4());
			}
			return 0;
		});

		if (err != 0) {
			return EAI_FAIL;
		}
		else if (!result.ips.empty()) {
			return 0;
		}
		else {
			return EAI_NONAME;
		}
	}
}

EXPORT int getnameinfo(
	const struct sockaddr* __restrict addr,
	socklen_t addr_len,
	char* host,
	socklen_t host_len,
	char* serv,
	socklen_t serv_len,
	int flags) {
	uint8_t addr_buf[16];
	uint16_t port;
	switch (addr->sa_family) {
		case AF_INET:
		{
			if (addr_len < sizeof(sockaddr_in)) {
				return EAI_FAMILY;
			}
			memcpy(addr_buf, &reinterpret_cast<const sockaddr_in*>(addr)->sin_addr, 4);
			port = reinterpret_cast<const sockaddr_in*>(addr)->sin_port;
			break;
		}
		case AF_INET6:
		{
			if (addr_len < sizeof(sockaddr_in6)) {
				return EAI_FAMILY;
			}
			memcpy(addr_buf, &reinterpret_cast<const sockaddr_in6*>(addr)->sin6_addr, 16);
			port = reinterpret_cast<const sockaddr_in6*>(addr)->sin6_port;
			break;
		}
		default:
			return EAI_FAMILY;
	}

	if (serv && serv_len) {
		if (flags & NI_NUMERICSERV) {
			size_t size = snprintf(serv, serv_len, "%u", port);
			if (serv_len < size + 1) {
				return EAI_OVERFLOW;
			}
		}
		else {
			if (auto res = resolve_port_in_services(port)) {
				if (serv_len < res->size() + 1) {
					return EAI_OVERFLOW;
				}
				memcpy(serv, res->data(), res->size() + 1);
			}
			else {
				size_t size = snprintf(serv, serv_len, "%u", port);
				if (serv_len < size + 1) {
					return EAI_OVERFLOW;
				}
			}
		}
	}

	if (host && host_len) {
		char addr_str[INET6_ADDRSTRLEN];
		__ensure(inet_ntop(addr->sa_family, addr_buf, addr_str, sizeof(addr_str)));

		hz::string_view addr_str_view {addr_str};

		if (!(flags & NI_NUMERICHOST)) {
			if (auto res = resolve_addr_in_hosts(addr_buf, addr->sa_family == AF_INET6)) {
				if (host_len < res->size() + 1) {
					return EAI_OVERFLOW;
				}
				memcpy(host, res->data(), res->size() + 1);
				return 0;
			}
			else {
				auto dns_res = resolve_addr_dns(addr_str, addr->sa_family == AF_INET6, host, host_len);
				if (dns_res != EAI_NONAME) {
					return dns_res;
				}
			}
		}

		if (flags & NI_NAMEREQD) {
			return EAI_NONAME;
		}

		if (host_len < addr_str_view.size() + 1) {
			return EAI_OVERFLOW;
		}
		memcpy(host, addr_str_view.data(), addr_str_view.size() + 1);
	}

	return 0;
}

namespace {
	constexpr in_addr IPV4_LOOPBACK {
		.s_addr = 127 | 0 << 8 | 0 << 16 | 1 << 24
	};
	constexpr in6_addr IPV6_LOOPBACK {
		.__in6_u {
			.__u6_addr8 {
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
			}
		}
	};
	constexpr in_addr IPV4_ANY {};
	constexpr in6_addr IPV6_ANY {};
}

EXPORT int getaddrinfo(
	const char* __restrict node_name,
	const char* __restrict service_name,
	const struct addrinfo* __restrict hints,
	struct addrinfo** __restrict res) {
	if (!node_name && !service_name) {
		return EAI_NONAME;
	}

	int flags = AI_V4MAPPED | AI_ADDRCONFIG;
	int socket_type = 0;
	int protocol = 0;
	int family = AF_UNSPEC;
	if (hints) {
		flags = hints->ai_flags;
		socket_type = hints->ai_socktype;
		protocol = hints->ai_protocol;
		family = hints->ai_family;
	}

	hz::string<Allocator> resolved_node_name {Allocator {}};

	bool use_ipv4 = family == AF_INET || family == AF_UNSPEC;
	bool use_ipv6 = family == AF_INET6 || family == AF_UNSPEC;
	if (use_ipv6 && !(flags & AI_V4MAPPED)) {
		use_ipv4 = false;
	}

	NameResolveResult result {};

	if (flags & AI_ADDRCONFIG) {
		int sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0) {
			use_ipv4 = false;
		}
		else {
			constexpr sockaddr_in loopback {
				.sin_family = AF_INET,
				.sin_port = 52000,
				.sin_addr = IPV4_LOOPBACK,
				.sin_zero {}
			};
			int err = connect(sock, reinterpret_cast<const sockaddr*>(&loopback), sizeof(loopback));
			if (err != 0) {
				use_ipv4 = false;
			}
			close(sock);
		}

		sock = socket(AF_INET6, SOCK_DGRAM, 0);
		if (sock < 0) {
			use_ipv6 = false;
		}
		else {
			constexpr sockaddr_in6 loopback {
				.sin6_family = AF_INET6,
				.sin6_port = 52000,
				.sin6_flowinfo {},
				.sin6_addr = IPV6_LOOPBACK,
				.sin6_scope_id {}
			};
			int err = connect(sock, reinterpret_cast<const sockaddr*>(&loopback), sizeof(loopback));
			if (err != 0) {
				use_ipv6 = false;
			}
			close(sock);
		}
	}

	if (node_name) {
		if (flags & AI_NUMERICHOST) {
			if (use_ipv4 && family == AF_INET) {
				if (auto ipv4 = parse_ipv4(node_name)) {
					result.ips.push_back(*ipv4);
				}
				else {
					return EAI_NONAME;
				}
			}
			else if (use_ipv6 && family == AF_INET6) {
				if (auto ipv6 = parse_ipv6(node_name)) {
					result.ips.push_back(*ipv6);
				}
				else {
					return EAI_NONAME;
				}
			}
			else if (family == AF_UNSPEC) {
				if (use_ipv4) {
					if (auto ipv4 = parse_ipv4(node_name)) {
						result.ips.push_back(*ipv4);
						family = AF_INET;
					}
				}
				if (use_ipv6 && family == AF_UNSPEC) {
					if (auto ipv6 = parse_ipv6(node_name)) {
						result.ips.push_back(*ipv6);
						family = AF_INET6;
					}
				}

				if (family == AF_UNSPEC) {
					return EAI_NONAME;
				}
			}
			else {
				return EAI_NONAME;
			}
		}
		else if (flags & AI_CANONNAME) {
			hz::string_view node_name_str = node_name;
			int err = resolve_canon_dns(node_name_str, resolved_node_name);
			if (err != 0) {
				resolved_node_name = node_name_str;
			}
		}
		else {
			resolved_node_name = node_name;
		}

		if (result.ips.empty()) {
			if (use_ipv6) {
				resolve_name_in_hosts(resolved_node_name, true, result);
				resolve_name_dns(resolved_node_name, true, result);
			}
			if (use_ipv4) {
				resolve_name_in_hosts(resolved_node_name, false, result);
				int err = resolve_name_dns(resolved_node_name, false, result);
				if (result.ips.empty() && err) {
					return err;
				}
			}

			if (result.ips.empty()) {
				return EAI_NONAME;
			}
		}
	}
	else {
		if (use_ipv6) {
			in6_addr addr = (flags & AI_PASSIVE) ? IPV6_ANY : IPV6_LOOPBACK;
			result.ips.push_back(addr);
		}
		if (use_ipv4) {
			in_addr addr = (flags & AI_PASSIVE) ? IPV4_ANY : IPV4_LOOPBACK;
			result.ips.push_back(addr);
		}
	}

	uint16_t port = 0;

	if (service_name) {
		hz::string_view service_view {service_name};
		if (flags & AI_NUMERICSERV) {
			size_t count;
			port = hz::to_integer<uint16_t>(service_view, 10, &count);
			if (count != service_view.size()) {
				return EAI_NONAME;
			}
		}
		else {
			size_t count = 0;
			auto value = hz::to_integer<uint16_t>(service_view, 10, &count);
			if (count == service_view.size()) {
				port = value;
			}
			else {
				if (auto resolved = resolve_name_in_services(
					service_view,
					socket_type == SOCK_STREAM || !socket_type)) {
					port = resolved->first;
					socket_type = resolved->second ? SOCK_STREAM : SOCK_DGRAM;
				}
				else {
					return EAI_SERVICE;
				}
			}
		}
	}

	addrinfo* start = nullptr;
	addrinfo* prev = nullptr;

	for (auto& ip : result.ips) {
		char* canon_name = nullptr;
		if (!resolved_node_name.empty()) {
			canon_name = new char[resolved_node_name.size() + 1];
			memcpy(canon_name, resolved_node_name.data(), resolved_node_name.size() + 1);
		}

		if (auto ipv6 = ip.get<in6_addr>()) {
			auto* addr = new sockaddr_in6 {
				.sin6_family = AF_INET6,
				.sin6_port = port,
				.sin6_flowinfo {},
				.sin6_addr = *ipv6,
				.sin6_scope_id {}
			};

			auto* info = new addrinfo {
				.ai_flags = 0,
				.ai_family = AF_INET6,
				.ai_socktype = socket_type,
				.ai_protocol = protocol,
				.ai_addrlen = sizeof(sockaddr_in6),
				.ai_addr = reinterpret_cast<sockaddr*>(addr),
				.ai_canonname = canon_name,
				.ai_next = nullptr
			};

			if (prev) {
				prev->ai_next = info;
			}
			prev = info;
		}
		else if (auto ipv4 = ip.get<in_addr>()) {
			auto* addr = new sockaddr_in {
				.sin_family = AF_INET,
				.sin_port = port,
				.sin_addr = *ipv4,
				.sin_zero {}
			};

			auto* info = new addrinfo {
				.ai_flags = 0,
				.ai_family = AF_INET,
				.ai_socktype = socket_type,
				.ai_protocol = protocol,
				.ai_addrlen = sizeof(sockaddr_in),
				.ai_addr = reinterpret_cast<sockaddr*>(addr),
				.ai_canonname = canon_name,
				.ai_next = nullptr
			};

			if (prev) {
				prev->ai_next = info;
			}
			prev = info;
		}

		if (!start) {
			start = prev;
		}
	}

	*res = start;
	return 0;
}

EXPORT void freeaddrinfo(struct addrinfo* ai) {
	while (ai) {
		auto* next = ai->ai_next;
		if (ai->ai_family == AF_INET6) {
			delete reinterpret_cast<sockaddr_in6*>(ai->ai_addr);
		}
		else if (ai->ai_family == AF_INET) {
			delete reinterpret_cast<sockaddr_in*>(ai->ai_addr);
		}
		delete[] ai->ai_canonname;
		delete ai;
		ai = next;
	}
}

EXPORT hostent* gethostbyname(const char* name) {
	static hostent entry {};

	delete[] entry.h_name;
	entry.h_name = nullptr;

	if (entry.h_aliases) {
		for (auto* ptr = entry.h_aliases; *ptr; ++ptr) {
			delete[] *ptr;
		}
		delete entry.h_aliases;
		entry.h_aliases = nullptr;
	}

	if (entry.h_addr_list) {
		for (auto* ptr = entry.h_addr_list; *ptr; ++ptr) {
			delete[] *ptr;
		}
		delete[] entry.h_addr_list;
	}

	hz::string_view name_view {name};
	if (auto ipv4 = parse_ipv4(name_view)) {
		entry.h_name = new char[name_view.size() + 1];
		memcpy(entry.h_name, name, name_view.size() + 1);
		entry.h_aliases = nullptr;
		entry.h_addrtype = AF_INET;
		entry.h_length = 4;
		entry.h_addr_list = new char*[2] {};
		*entry.h_addr_list = new char[4];
		memcpy(*entry.h_addr_list, &ipv4.value(), 4);
		return &entry;
	}

	hz::string<Allocator> canon_name {Allocator {}};
	int res = resolve_canon_dns(name_view, canon_name);
	if (res != 0) {
		canon_name = name_view;
	}

	NameResolveResult result {};
	resolve_name_in_hosts(canon_name, true, result);
	resolve_name_dns(canon_name, true, result);
	int err = resolve_name_dns(canon_name, false, result);
	if (result.ips.empty() && err) {
		h_errno = NO_RECOVERY;
		return nullptr;
	}

	if (result.ips.empty()) {
		h_errno = HOST_NOT_FOUND;
		return nullptr;
	}

	entry.h_name = new char[canon_name.size() + 1];
	memcpy(entry.h_name, canon_name.data(), canon_name.size() + 1);
	if (canon_name != name_view) {
		entry.h_aliases = new char*[2] {};
		*entry.h_aliases = new char[name_view.size() + 1];
		memcpy(*entry.h_aliases, name_view.data(), name_view.size() + 1);
	}

	size_t ipv6_count = 0;
	size_t ipv4_count = 0;
	for (auto& ip : result.ips) {
		if (ip.get<in6_addr>()) {
			++ipv6_count;
		}
		else {
			++ipv4_count;
		}
	}

	if (ipv4_count) {
		entry.h_addrtype = AF_INET;
		entry.h_length = 4;
		entry.h_addr_list = new char*[ipv4_count + 1] {};

		auto* ptr = entry.h_addr_list;
		for (auto& ip : result.ips) {
			if (auto ipv4 = ip.get<in_addr>()) {
				auto* loc = new char[4];
				memcpy(loc, ipv4, 4);
				*ptr++ = loc;
			}
		}
	}
	else {
		entry.h_addrtype = AF_INET6;
		entry.h_length = 16;
		entry.h_addr_list = new char*[ipv6_count + 1] {};

		auto* ptr = entry.h_addr_list;
		for (auto& ip : result.ips) {
			if (auto ipv6 = ip.get<in6_addr>()) {
				auto* loc = new char[16];
				memcpy(loc, ipv6, 16);
				*ptr++ = loc;
			}
		}
	}

	return &entry;
}

EXPORT struct hostent* gethostbyaddr(const void* addr, socklen_t len, int type) {
	__ensure(!"gethostbyaddr is not implemented");
}

EXPORT servent* getservbyname(const char* name, const char* proto) {
	static servent entry {};

	delete[] entry.s_name;
	entry.s_name = nullptr;

	if (entry.s_aliases) {
		for (auto* ptr = entry.s_aliases; *ptr; ++ptr) {
			delete[] *ptr;
		}
		delete entry.s_aliases;
		entry.s_aliases = nullptr;
	}

	delete entry.s_proto;
	entry.s_proto = nullptr;

	hz::string_view name_view {name};
	hz::string_view proto_view {};
	if (proto) {
		proto_view = proto;
	}

	FILE* file = fopen("/etc/services", "r");
	if (!file) {
		return nullptr;
	}

	char line_buf[256];
	while (fgets(line_buf, 256, file)) {
		hz::string_view line {line_buf};
		if (auto comment_start = line.find('#'); comment_start != hz::string_view::npos) {
			line = line.substr_abs(0, comment_start);
		}

		auto name_end = line.find([](char c) {
			return isspace(c);
		});

		auto service_name = line.substr_abs(0, name_end);

		auto port_start = line.find([](char c) {
			return !isspace(c);
		}, name_end);
		if (port_start == hz::string_view::npos) {
			continue;
		}
		auto port_end = line.find([](char c) {
			return isspace(c);
		}, port_start);
		auto port = line.substr_abs(port_start, port_end);

		if (service_name == name_view) {
			size_t count;
			auto real_port = hz::to_integer<uint16_t>(port, 10, &count);
			auto end = port.substr(count + 1);

			if (!proto_view.size() || (proto_view == "tcp" && end == "tcp") || (proto_view == "udp" && end == "udp")) {
				fclose(file);

				entry.s_name = new char[name_view.size() + 1];
				memcpy(entry.s_name, name_view.data(), name_view.size() + 1);

				size_t alias_count = 0;

				size_t offset = port_end;
				while (true) {
					auto alias_start = line.find([](char c) {
						return !isspace(c);
					}, offset);
					auto alias_end = line.find([](char c) {
						return isspace(c);
					}, alias_start);

					auto alias = line.substr_abs(alias_start, alias_end);
					if (alias.size() == 0) {
						break;
					}

					if (!alias_count) {
						entry.s_aliases = new char*[2];
						entry.s_aliases[1] = nullptr;
					}
					else {
						auto* new_aliases = new char*[alias_count + 2];
						memcpy(new_aliases, entry.s_aliases, alias_count * sizeof(char*));
						entry.s_aliases = new_aliases;
						entry.s_aliases[alias_count + 1] = nullptr;
					}

					entry.s_aliases[alias_count++] = new char[alias.size() + 1];
					memcpy(entry.s_aliases[alias_count - 1], alias.data(), alias.size());
					entry.s_aliases[alias_count - 1][alias.size()] = 0;

					if (alias_end == hz::string_view::npos) {
						break;
					}
					else {
						offset = alias_end + 1;
					}
				}

				entry.s_port = real_port;
				entry.s_proto = new char[end.size() + 1];
				memcpy(entry.s_proto, end.data(), end.size());
				entry.s_proto[end.size()] = 0;

				return &entry;
			}
		}
	}

	fclose(file);
	return nullptr;
}

namespace {
	thread_local int __h_errno = 0;
}

EXPORT int* __h_errno_location() {
	return &__h_errno;
}

EXPORT const char* gai_strerror(int err_code) {
	switch (err_code) {
		case EAI_BADFLAGS:
			return "Bad value for ai_flags";
		case EAI_NONAME:
			return "Name or service not known";
		case EAI_AGAIN:
			return "Temporary failure in name resolution";
		case EAI_FAIL:
			return "Non-recoverable failure in name resolution";
		case EAI_FAMILY:
			return "ai_family not supported";
		case EAI_SOCKTYPE:
			return "ai_socktype not supported";
		case EAI_SERVICE:
			return "Service not supported for ai_socktype";
		case EAI_MEMORY:
			return "Memory allocation failure";
		case EAI_SYSTEM:
			return "System error";
		case EAI_OVERFLOW:
			return "Result too large for supplied buffer";
		case EAI_NODATA:
			return "No address associated with hostname";
		case EAI_ADDRFAMILY:
			return "Address family for hostname not supported";
		case EAI_INPROGRESS:
			return "Processing request in progress";
		case EAI_CANCELED:
			return "Request canceled";
		case EAI_NOTCANCELED:
			return "Request not canceled";
		case EAI_ALLDONE:
			return "All requests done";
		case EAI_INTR:
			return "Interrupted by a signal";
		case EAI_IDN_ENCODE:
			return "Parameter string not correctly encoded";
		default:
			return "Unknown error";
	}
}
