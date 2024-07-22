#include "grp.h"
#include "utils.hpp"
#include "stdio.h"
#include "errno.h"
#include "allocator.hpp"
#include <hz/string_utils.hpp>
#include <hz/string.hpp>
#include <hz/vector.hpp>

namespace {
	hz::string<Allocator> GRP_STR_AREA {Allocator {}};
	hz::vector<char*, Allocator> GRP_PTR_AREA {Allocator {}};
	FILE* GLOBAL_FILE;

	struct group_view {
		hz::string_view name;
		hz::string_view password;
		gid_t gid {};
		hz::string_view member_list {};
	};

	bool parse_entry(hz::string_view line, group_view& view) {
		size_t offset = 0;
		auto parse_segment = [&](hz::string_view& res) {
			size_t next_offset = line.find(':', offset);
			res = line.substr_abs(offset, next_offset);
			if (next_offset == hz::string_view::npos) {
				return false;
			}
			else {
				offset = next_offset + 1;
				return true;
			}
		};

		hz::string_view name;
		if (!parse_segment(name)) {
			return false;
		}
		hz::string_view password;
		if (!parse_segment(password)) {
			return false;
		}
		hz::string_view group_id;
		if (!parse_segment(group_id)) {
			return false;
		}
		hz::string_view member_list;
		if (parse_segment(member_list)) {
			return false;
		}
		if (member_list.ends_with('\n')) {
			member_list = member_list.substr(0, member_list.size() - 1);
		}

		size_t count;
		auto gid = hz::to_integer<gid_t>(group_id, count);
		if (count != group_id.size()) {
			return false;
		}

		view.name = name;
		view.password = password;
		view.gid = gid;
		view.member_list = member_list;
		return true;
	}

	void fill_group(group_view view, group& res) {
		GRP_STR_AREA = view.name;
		GRP_STR_AREA.push_back(0);
		auto grp_passwd_offset = GRP_STR_AREA.size();
		GRP_STR_AREA += view.password;
		GRP_STR_AREA.push_back(0);

		hz::vector<size_t, Allocator> offsets {Allocator {}};

		size_t offset = 0;
		while (true) {
			auto end = view.member_list.find(',', offset);
			auto user_name = view.member_list.substr_abs(offset, end);
			offsets.push_back(GRP_STR_AREA.size());
			GRP_STR_AREA += user_name;
			GRP_STR_AREA.push_back(0);

			if (end != hz::string_view::npos) {
				offset = end + 1;
			}
			else {
				break;
			}
		}

		GRP_PTR_AREA.clear();
		for (auto off : offsets) {
			GRP_PTR_AREA.push_back(GRP_STR_AREA.data() + off);
		}
		GRP_PTR_AREA.push_back(nullptr);

		res.gr_name = GRP_STR_AREA.data();
		res.gr_passwd = GRP_STR_AREA.data() + grp_passwd_offset;
		res.gr_gid = view.gid;
		res.gr_mem = GRP_PTR_AREA.data();
	}

	bool open_global_file() {
		if (!GLOBAL_FILE) {
			GLOBAL_FILE = fopen("/etc/group", "r");
			if (!GLOBAL_FILE) {
				errno = EIO;
				return false;
			}
		}

		return true;
	}
}

EXPORT group* getgrnam(const char* name) {
	static group grp {};

	FILE* file = fopen("/etc/group", "r");
	if (!file) {
		errno = EIO;
		return nullptr;
	}

	hz::string_view name_str {name};

	char buf[NSS_BUFLEN_GROUP];
	while (fgets(buf, NSS_BUFLEN_GROUP, file)) {
		group_view view;
		if (!parse_entry(buf, view)) {
			continue;
		}
		if (view.name == name_str) {
			fill_group(view, grp);
			return &grp;
		}
	}

	if (ferror(file)) {
		fclose(file);
		errno = EIO;
		return nullptr;
	}

	fclose(file);
	errno = ENOENT;
	return nullptr;
}

EXPORT group* getgrgid(gid_t gid) {
	static group grp;

	FILE* file = fopen("/etc/group", "r");
	if (!file) {
		errno = EIO;
		return nullptr;
	}

	char buf[NSS_BUFLEN_GROUP];
	while (fgets(buf, NSS_BUFLEN_GROUP, file)) {
		group_view view;
		if (!parse_entry(buf, view)) {
			continue;
		}
		if (view.gid == gid) {
			fill_group(view, grp);
			return &grp;
		}
	}

	if (ferror(file)) {
		fclose(file);
		errno = EIO;
		return nullptr;
	}

	fclose(file);
	errno = ENOENT;
	return nullptr;
}

EXPORT void setgrent() {
	if (!open_global_file()) {
		return;
	}
	rewind(GLOBAL_FILE);
}

EXPORT group* getgrent() {
	static group grp;

	if (!open_global_file()){
		return nullptr;
	}

	char buf[NSS_BUFLEN_GROUP];
	while (fgets(buf, NSS_BUFLEN_GROUP, GLOBAL_FILE)) {
		group_view view;
		if (!parse_entry(buf, view)) {
			continue;
		}
		fill_group(view, grp);
		return &grp;
	}

	errno = ENOENT;
	return nullptr;
}

EXPORT void endgrent() {
	if (GLOBAL_FILE) {
		if (fclose(GLOBAL_FILE) != 0) {
			errno = EIO;
		}
		GLOBAL_FILE = nullptr;
	}
}
