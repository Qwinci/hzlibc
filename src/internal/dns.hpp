#pragma once
#include "allocator.hpp"
#include "netinet/in.h"
#include "errno.h"
#include "sys/time.h"
#include "unistd.h"
#include <stdint.h>
#include <hz/vector.hpp>
#include <hz/variant.hpp>
#include <hz/string.hpp>

#define memcpy __builtin_memcpy

namespace dns {
	struct Header {
		uint16_t transaction_id;
		uint16_t flags;
		uint16_t num_questions;
		uint16_t num_answers;
		uint16_t num_authority_rr;
		uint16_t num_additional_rr;
	};

	namespace flags {
		static constexpr uint16_t RD = 1 << 0;
	}

	namespace qtype {
		static constexpr uint16_t A = 1;
		static constexpr uint16_t CNAME = 5;
		static constexpr uint16_t PTR = 12;
		static constexpr uint16_t AAAA = 28;
	}

	hz::vector<hz::variant<in_addr, in6_addr>, Allocator> get_nameservers();

	struct Ptr {
		in_addr parse_ipv4();
		in6_addr parse_ipv6();
		
		hz::string<Allocator> parse_dns_name();
		int expand_dns_name(const char* msg_end, char* dest, size_t dest_size);

		const char* base;
		const char* ptr;
	};

	struct Query {
		explicit Query(uint16_t type);

		void add_segment(hz::string_view segment) {
			__ensure(segment.size() <= 0xFF);
			req += static_cast<char>(segment.size());
			req += segment;
		}

		template<typename F> requires requires(F func, Ptr data_ptr, uint16_t qtype) {
			{ func(qtype, data_ptr) } -> hz::same_as<int>;
		}
		int send(F on_answer) {
			char res[256];
			if (auto err = send(res, sizeof(res))) {
				return err;
			}

			auto* res_hdr = reinterpret_cast<dns::Header*>(res);
			auto* ptr = reinterpret_cast<const char*>(&res_hdr[1]);
			for (int i = 0; i < hz::to_ne_from_be(res_hdr->num_questions); ++i) {
				while (true) {
					auto len = static_cast<uint8_t>(*ptr++);
					if (len == 0) {
						break;
					}
					ptr += len;
				}
				ptr += 4;
			}

			for (int i = 0; i < hz::to_ne_from_be(res_hdr->num_answers); ++i) {
				Ptr dns_ptr {
					.base = reinterpret_cast<char*>(res_hdr),
					.ptr = ptr
				};
				dns_ptr.parse_dns_name();
				ptr = dns_ptr.ptr;
				uint16_t ans_type = ptr[0] << 8 | ptr[1];
				uint16_t rd_len = ptr[8] << 8 | ptr[9];
				ptr += 10;

				dns_ptr.ptr = ptr;
				if (auto status = on_answer(ans_type, dns_ptr)) {
					return status;
				}
				ptr += rd_len;
			}

			return 0;
		}

		int send(void* response, size_t response_size);

		hz::string<Allocator> req {Allocator {}};
		uint16_t transaction_id {};
		uint16_t type {};
	};
}
