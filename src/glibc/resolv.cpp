#include "resolv.h"
#include "utils.hpp"
#include "internal/dns.hpp"
#include "netdb.h"

EXPORT int res_search(
	const char* domain_name,
	int clazz,
	int type,
	unsigned char* answer,
	int answer_len) {
	__ensure(!"res_search is not implemented");
}

EXPORT int res_query(
	const char* domain_name,
	int clazz,
	int type,
	unsigned char* answer,
	int answer_len) {
	return res_nquery(nullptr, domain_name, clazz, type, answer, answer_len);
}

EXPORT int res_ninit(res_state) {
	return 0;
}

EXPORT void res_nclose(res_state) {

}

EXPORT int res_nquery(
	res_state,
	const char* domain_name,
	int clazz,
	int type,
	unsigned char* answer,
	int answer_len) {
	if (clazz != 1) {
		h_errno = EAI_SYSTEM;
		return -1;
	}

	hz::string_view name {domain_name};

	dns::Query query {static_cast<uint16_t>(type)};

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

	if (auto err = query.send(answer, answer_len)) {
		switch (err) {
			case EOVERFLOW:
				h_errno = EAI_OVERFLOW;
				break;
			default:
				h_errno = EAI_SYSTEM;
				break;
		}
		return -1;
	}
	return 0;
}

EXPORT int dn_expand(
	const unsigned char* msg,
	const unsigned char* eom_orig,
	const unsigned char* comp_dn,
	char* expanded_dn,
	int length) {
	dns::Ptr ptr {reinterpret_cast<const char*>(msg), reinterpret_cast<const char*>(comp_dn)};
	if (auto err = ptr.expand_dns_name(
		reinterpret_cast<const char*>(eom_orig),
		expanded_dn,
		static_cast<size_t>(length))) {
		errno = err;
		return -1;
	}
	return static_cast<int>(ptr.ptr - ptr.base);
}

ALIAS(res_ninit, __res_ninit);
ALIAS(res_nclose, __res_nclose);
ALIAS(res_nquery, __res_nquery);
ALIAS(dn_expand, __dn_expand);
