#include "libintl.h"
#include "utils.hpp"
#include "allocator.hpp"
#include <hz/string.hpp>
#include <hz/vector.hpp>

namespace {
	struct Domain {
		hz::string<Allocator> name;
		hz::string<Allocator> dir_name;
	};

	[[gnu::init_priority(101)]] hz::string<Allocator> DOMAIN {Allocator {}};
	hz::vector<Domain, Allocator> DOMAINS {Allocator {}};

	[[gnu::constructor(102)]] void init_domains() {
		DOMAIN = "messages";
	}
}

EXPORT char* textdomain(const char* domain_name) {
	if (domain_name) {
		DOMAIN = domain_name;
	}
	return DOMAIN.data();
}

EXPORT char* bindtextdomain(const char* domain_name, const char* dir_name) {
	hz::string_view domain_name_str {domain_name};
	for (auto& domain : DOMAINS) {
		if (domain.name == domain_name_str) {
			domain.dir_name = dir_name;
			return domain.dir_name.data();
		}
	}

	hz::string<Allocator> name_str {Allocator {}};
	hz::string<Allocator> dir_str {Allocator {}};
	name_str = domain_name_str;
	dir_str = dir_name;
	DOMAINS.push_back(Domain {
		.name {std::move(name_str)},
		.dir_name {std::move(dir_str)}
	});
	return DOMAINS.back().dir_name.data();
}

EXPORT char* bind_textdomain_codeset(const char* domain_name, const char* code_set) {
	println(
		"bind_textdomain_codeset domain '",
		domain_name,
		"' set '",
		code_set,
		"' is not implemented");
	return const_cast<char*>("UTF-8");
}

EXPORT char* gettext(const char* msg_id) {
	println("gettext is not properly implemented");
	return const_cast<char*>(msg_id);
}

EXPORT char* dgettext(const char* domain_name, const char* msg_id) {
	println("dgettext is not properly implemented");
	return const_cast<char*>(msg_id);
}

EXPORT char* dcgettext(const char* domain_name, const char* msg_id, int category) {
	println("dcgettext is not properly implemented");
	return const_cast<char*>(msg_id);
}

EXPORT char* ngettext(const char* msg_id, const char* msg_id_plural, unsigned long n) {
	println("ngettext is not properly implemented");
	return const_cast<char*>(msg_id);
}
