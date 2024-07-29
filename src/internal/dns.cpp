#include "dns.hpp"
#include "stdio.h"
#include "posix/net_utils.hpp"

namespace dns {
	hz::vector<hz::variant<in_addr, in6_addr>, Allocator> get_nameservers() {
		hz::vector<hz::variant<in_addr, in6_addr>, Allocator> res {Allocator {}};

		FILE* file = fopen("/etc/resolv.conf", "r");
		if (!file) {
			return res;
		}

		char buf[256];
		while (fgets(buf, 256, file)) {
			hz::string_view line {buf};
			line = line.substr_abs(0, line.size() - 1);
			if (!line.starts_with("nameserver ")) {
				continue;
			}

			auto ip_str = line.substr_abs(sizeof("nameserver ") - 1);
			if (auto ipv4 = parse_ipv4(ip_str)) {
				res.push_back(*ipv4);
			}
			else if (auto ipv6 = parse_ipv6(ip_str)) {
				res.push_back(*ipv6);
			}
		}

		fclose(file);
		return res;
	}

	in_addr Ptr::parse_ipv4() {
		uint32_t value;
		memcpy(&value, ptr, 4);
		ptr += 4;
		return {.s_addr = hz::to_ne_from_be(value)};
	}

	in6_addr Ptr::parse_ipv6() {
		in6_addr translated {};
		for (int j = 0; j < 8; ++j) {
			uint16_t value;
			memcpy(&value, ptr + j * 2, 2);
			translated.s6_addr16[j] = hz::to_ne_from_be(value);
		}
		ptr += 16;
		return translated;
	}

	hz::string<Allocator> Ptr::parse_dns_name() {
		hz::string<Allocator> name {Allocator {}};

		while (true) {
			auto len = static_cast<uint8_t>(*ptr++);
			if ((len & 0b11000000) == 0b11000000) {
				uint8_t str_offset = (len & 0b111111) << 8 | *ptr++;

				auto* ptr2 = base + str_offset;
				while (true) {
					auto len2 = static_cast<uint8_t>(*ptr2++);
					__ensure((len2 & 0b11000000) != 0b11000000);
					if (!len2) {
						break;
					}
					name += hz::string_view {ptr2, len2};
					ptr2 += len2;
					if (*ptr2) {
						name += '.';
					}
				}

				break;
			}
			else {
				if (!len) {
					break;
				}
				name += hz::string_view {ptr, len};
				ptr += len;
				if (*ptr) {
					name += '.';
				}
			}
		}

		return name;
	}

	int Ptr::expand_dns_name(const char* msg_end, char* dest, size_t dest_size) {
		while (true) {
			if (ptr >= msg_end) {
				return EMSGSIZE;
			}

			auto len = static_cast<uint8_t>(*ptr++);
			if ((len & 0b11000000) == 0b11000000) {
				uint8_t str_offset = (len & 0b111111) << 8 | *ptr++;

				auto* ptr2 = base + str_offset;
				while (true) {
					if (ptr2 >= msg_end) {
						return EMSGSIZE;
					}
					auto len2 = static_cast<uint8_t>(*ptr2++);
					__ensure((len2 & 0b11000000) != 0b11000000);
					if (!len2) {
						break;
					}
					if (ptr2 + len2 > msg_end) {
						return EMSGSIZE;
					}
					if (dest_size < len2) {
						return EOVERFLOW;
					}
					memcpy(dest, ptr2, len2);
					dest_size -= len2;
					ptr2 += len2;
					dest += len2;
					if (*ptr2) {
						if (!dest_size) {
							return EOVERFLOW;
						}
						*dest++ = '.';
						--dest_size;
					}
				}

				break;
			}
			else {
				if (!len) {
					break;
				}
				if (ptr + len > msg_end) {
					return EMSGSIZE;
				}
				if (dest_size < len) {
					return EOVERFLOW;
				}
				memcpy(dest, ptr, len);
				dest_size -= len;
				ptr += len;
				dest += len;
				if (*ptr) {
					if (!dest_size) {
						return EOVERFLOW;
					}
					*dest++ = '.';
					--dest_size;
				}
			}
		}

		if (!dest_size) {
			return EOVERFLOW;
		}
		*dest = 0;

		return 0;
	}

	Query::Query(uint16_t type) : type {type} {
		dns::Header hdr {
			.transaction_id = hz::to_be<uint16_t>(1234),
		    .flags = dns::flags::RD,
			.num_questions = hz::to_be<uint16_t>(1),
		    .num_answers = 0,
			.num_authority_rr = 0,
			.num_additional_rr = 0
		};
		
		req.resize(sizeof(hdr));
		memcpy(req.data(), &hdr, sizeof(hdr));
		transaction_id = hdr.transaction_id;
	}

	int Query::send(void* response, size_t response_size) {
		req += '\0';

		// qtype
		req += static_cast<char>(type >> 8);
		req += static_cast<char>(type & 0xFF);
		// class IN
		req += 0;
		req += 1;

		auto name_servers = get_nameservers();
		if (name_servers.empty()) {
			in_addr localhost_addr {
				.s_addr = 127 | 0 << 8 | 0 << 16 | 1 << 24
			};
			name_servers.push_back(localhost_addr);
		}

		int last_err = 0;

		for (int tries = 0; tries < 3; ++tries) {
			for (auto& name_server : name_servers) {
				char dns_addr_buf[sizeof(sockaddr_in6)];
				size_t dns_addr_size;
				if (auto ipv6 = name_server.get<in6_addr>()) {
					sockaddr_in6 dns_addr {};
					dns_addr.sin6_family = AF_INET6;
					dns_addr.sin6_port = hz::to_be(in_port_t {53});
					dns_addr.sin6_addr = *ipv6;
					memcpy(dns_addr_buf, &dns_addr, sizeof(dns_addr));
					dns_addr_size = sizeof(dns_addr);
				}
				else if (auto ipv4 = name_server.get<in_addr>()) {
					sockaddr_in dns_addr {};
					dns_addr.sin_family = AF_INET;
					dns_addr.sin_port = hz::to_be(in_port_t {53});
					dns_addr.sin_addr = *ipv4;
					memcpy(dns_addr_buf, &dns_addr, sizeof(dns_addr));
					dns_addr_size = sizeof(dns_addr);
				}
				else {
					__ensure(false);
				}

				int sock = socket(AF_INET, SOCK_DGRAM, 0);
				if (sock < 0) {
					return errno;
				}

				timeval timeout {
					.tv_sec = 3,
					.tv_usec = 0
				};
				setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

				auto count = sendto(
					sock,
					req.data(),
					req.size(),
					0,
					reinterpret_cast<sockaddr*>(dns_addr_buf),
					dns_addr_size);
				if (count < 0 || static_cast<size_t>(count) != req.size()) {
					close(sock);
					last_err = errno;
					continue;
				}

				char res[256];
				auto received = recvfrom(sock, res, sizeof(res), 0, nullptr, nullptr);
				if (received < 0) {
					last_err = errno;
					close(sock);
					continue;
				}
				else if (static_cast<size_t>(received) < sizeof(dns::Header)) {
					last_err = EINVAL;
					close(sock);
					continue;
				}

				close(sock);

				auto* res_hdr = reinterpret_cast<dns::Header*>(res);
				if (res_hdr->transaction_id != transaction_id) {
					return EINVAL;
				}

				if (response_size < static_cast<size_t>(received)) {
					return EOVERFLOW;
				}
				memcpy(response, res, static_cast<size_t>(received));
				return 0;
			}
		}

		return last_err;
	}
}

