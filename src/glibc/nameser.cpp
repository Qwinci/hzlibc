#include "arpa/nameser.h"
#include "utils.hpp"
#include "internal/dns.hpp"
#include <hz/bit.hpp>

EXPORT int ns_initparse(const unsigned char* msg, int msg_len, ns_msg* handle) {
	auto* hdr = reinterpret_cast<const dns::Header*>(msg);
	handle->_msg = msg;
	handle->_eom = msg + msg_len;
	handle->_id = hz::to_ne_from_be(hdr->transaction_id);
	handle->_flags = hz::to_ne_from_be(hdr->flags);
	handle->_counts[ns_s_qd] = hz::to_ne_from_be(hdr->num_questions);
	handle->_counts[ns_s_an] = hz::to_ne_from_be(hdr->num_answers);
	handle->_counts[ns_s_ns] = hz::to_ne_from_be(hdr->num_authority_rr);
	handle->_counts[ns_s_ar] = hz::to_ne_from_be(hdr->num_additional_rr);

	auto* ptr = reinterpret_cast<const char*>(&hdr[1]);
	handle->_sections[ns_s_qd] = reinterpret_cast<const unsigned char*>(ptr);
	for (int i = 0; i < handle->_counts[ns_s_qd]; ++i) {
		while (true) {
			auto len = static_cast<uint8_t>(*ptr++);
			if (len == 0) {
				break;
			}
			ptr += len;
		}
		ptr += 4;
	}

	handle->_sections[ns_s_an] = reinterpret_cast<const unsigned char*>(ptr);

	for (int i = ns_s_an; i < ns_s_max - 1; ++i) {
		for (int j = 0; j < handle->_counts[i]; ++j) {
			dns::Ptr dns_ptr {
				.base = reinterpret_cast<const char*>(hdr),
				.ptr = ptr
			};
			dns_ptr.parse_dns_name();
			ptr = dns_ptr.ptr;
			uint16_t rd_len = ptr[8] << 8 | ptr[9];
			ptr += 10 + rd_len;
		}

		handle->_sections[i + 1] = reinterpret_cast<const unsigned char*>(ptr);
	}

	return 0;
}

EXPORT int ns_parserr(ns_msg* handle, ns_sect section, int rr_num, ns_rr* rr) {
	auto* ptr = handle->_sections[section];
	auto count = handle->_counts[section];
	if (section >= count) {
		errno = EINVAL;
		return -1;
	}

	if (section == ns_s_qd) {
		for (int i = 0; i < rr_num; ++i) {
			while (true) {
				auto len = static_cast<uint8_t>(*ptr++);
				if (len == 0) {
					break;
				}
				ptr += len;
			}
			ptr += 4;
		}

		auto* name_ptr = rr->name;
		while (true) {
			auto len = static_cast<uint8_t>(*ptr++);
			if (len == 0) {
				*name_ptr = 0;
				break;
			}

			memcpy(name_ptr, ptr, len);
			name_ptr += len;
			ptr += len;
			if (*ptr) {
				*name_ptr++ = '.';
			}
		}

		rr->type = ns_get16(ptr);
		ptr += 2;
		rr->rr_class = ns_get16(ptr);
		rr->ttl = 0;
		rr->rdlength = 0;
		rr->rdata = nullptr;
	}
	else {
		for (int i = 0; i < rr_num; ++i) {
			dns::Ptr dns_ptr {
				.base = reinterpret_cast<const char*>(handle->_msg),
				.ptr = reinterpret_cast<const char*>(ptr)
			};
			dns_ptr.parse_dns_name();
			ptr = reinterpret_cast<const unsigned char*>(dns_ptr.ptr);
			uint16_t rd_len = ptr[8] << 8 | ptr[9];
			ptr += 10 + rd_len;
		}

		dns::Ptr dns_ptr {
			.base = reinterpret_cast<const char*>(handle->_msg),
			.ptr = reinterpret_cast<const char*>(ptr)
		};
		dns_ptr.expand_dns_name(
			reinterpret_cast<const char*>(handle->_eom),
			rr->name,
			sizeof(rr->name));
		ptr = reinterpret_cast<const unsigned char*>(dns_ptr.ptr);
		rr->type = ptr[0] << 8 | ptr[1];
		rr->rr_class = ptr[2] << 8 | ptr[3];
		rr->ttl = ptr[4] << 24 | ptr[5] << 16 | ptr[6] << 8 | ptr[7];
		rr->rdlength = ptr[8] << 8 | ptr[9];
		rr->rdata = ptr + 10;
	}

	return 0;
}

EXPORT unsigned int ns_get16(const unsigned char* ptr) {
	return hz::to_ne_from_be(*reinterpret_cast<const uint16_t*>(ptr));
}
