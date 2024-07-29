#include "grp.h"
#include "utils.hpp"
#include "stdio.h"
#include "errno.h"
#include "allocator.hpp"
#include "string.h"
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
		auto gid = hz::to_integer<gid_t>(group_id, 10, &count);
		if (count != group_id.size()) {
			return false;
		}

		view.name = name;
		view.password = password;
		view.gid = gid;
		view.member_list = member_list;
		return true;
	}

	int fill_group_in_area(group_view view, group& res, char* area, size_t area_size) {
		size_t member_offset = 0;
		size_t member_list_size = 0;
		size_t member_count = 0;
		while (true) {
			auto end = view.member_list.find(',', member_offset);
			auto user_name = view.member_list.substr_abs(member_offset, end);
			member_list_size += user_name.size() + 1;
			member_list_size += sizeof(char*);
			++member_count;

			if (end != hz::string_view::npos) {
				member_offset = end + 1;
			}
			else {
				break;
			}
		}

		size_t total =
			view.name.size() + 1 +
			view.password.size() + 1 +
			member_list_size + sizeof(char*);
		if (area_size < total) {
			return ERANGE;
		}

		auto* member_ptr_area = reinterpret_cast<char**>(area);

		size_t offset = (member_count + 1) * sizeof(char*);
		auto grp_name_offset = offset;
		memcpy(area, view.name.data(), view.name.size());
		offset += view.name.size() + 1;
		area[offset - 1] = 0;

		auto grp_passwd_offset = offset;
		memcpy(area + offset, view.password.data(), view.password.size());
		offset += view.password.size() + 1;
		area[offset - 1] = 0;

		member_offset = 0;
		member_count = 0;
		while (true) {
			auto end = view.member_list.find(',', member_offset);
			auto user_name = view.member_list.substr_abs(member_offset, end);

			memcpy(area + offset, user_name.data(), user_name.size());
			member_ptr_area[member_count++] = area + offset;
			offset += user_name.size() + 1;
			area[offset - 1] = 0;

			if (end != hz::string_view::npos) {
				member_offset = end + 1;
			}
			else {
				break;
			}
		}

		member_ptr_area[member_count] = nullptr;

		res.gr_name = area + grp_name_offset;
		res.gr_passwd = area + grp_passwd_offset;
		res.gr_gid = view.gid;
		res.gr_mem = reinterpret_cast<char**>(area);
		return 0;
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

EXPORT int getgrnam_r(
	const char* name,
	struct group* grp,
	char* buf,
	size_t buf_len,
	struct group** result) {
	FILE* file = fopen("/etc/group", "r");
	if (!file) {
		*result = nullptr;
		return EIO;
	}

	hz::string_view name_str {name};

	char line_buf[NSS_BUFLEN_GROUP];
	while (fgets(line_buf, NSS_BUFLEN_GROUP, file)) {
		group_view view;
		if (!parse_entry(line_buf, view)) {
			continue;
		}
		if (view.name == name_str) {
			if (auto err = fill_group_in_area(view, *grp, buf, buf_len)) {
				fclose(file);
				*result = nullptr;
				return err;
			}
			*result = grp;
			return 0;
		}
	}

	*result = nullptr;

	if (ferror(file)) {
		fclose(file);
		return EIO;
	}
	fclose(file);
	return 0;
}

EXPORT int getgrgid_r(
	gid_t gid,
	struct group* grp,
	char* buf,
	size_t buf_len,
	struct group** result) {
	FILE* file = fopen("/etc/group", "r");
	if (!file) {
		*result = nullptr;
		return EIO;
	}

	char line_buf[NSS_BUFLEN_GROUP];
	while (fgets(line_buf, NSS_BUFLEN_GROUP, file)) {
		group_view view;
		if (!parse_entry(line_buf, view)) {
			continue;
		}
		if (view.gid == gid) {
			if (auto err = fill_group_in_area(view, *grp, buf, buf_len)) {
				fclose(file);
				*result = nullptr;
				return err;
			}
			*result = grp;
			return 0;
		}
	}

	*result = nullptr;

	if (ferror(file)) {
		fclose(file);
		return EIO;
	}
	fclose(file);
	return 0;
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

// this is glibc, but it is here to avoid needing to make the group functions global
EXPORT int getgrouplist(const char* user, gid_t group, gid_t* groups, int* num_groups) {
	FILE* file = fopen("/etc/group", "r");
	if (!file) {
		errno = EIO;
		return -1;
	}

	hz::string_view user_str {user};

	int total_num_groups = 0;
	int ret = 0;
	int max_groups = *num_groups;
	bool contained = false;
	char line_buf[NSS_BUFLEN_GROUP];
	while (fgets(line_buf, NSS_BUFLEN_GROUP, file)) {
		group_view view;
		if (!parse_entry(line_buf, view)) {
			continue;
		}

		size_t offset = 0;
		while (true) {
			auto end = view.member_list.find(',', offset);
			auto user_name = view.member_list.substr_abs(offset, end);

			if (user_name == user_str) {
				if (total_num_groups < max_groups) {
					groups[total_num_groups] = view.gid;
				}
				else {
					ret = -1;
				}
				if (view.gid == group) {
					contained = true;
				}
				++total_num_groups;
				break;
			}

			if (end != hz::string_view::npos) {
				offset = end + 1;
			}
			else {
				break;
			}
		}
	}

	if (ferror(file)) {
		fclose(file);
		errno = EIO;
		return -1;
	}

	if (!contained) {
		if (total_num_groups < max_groups) {
			groups[total_num_groups] = group;
		}
		else {
			ret = -1;
		}
		++total_num_groups;
	}

	*num_groups = total_num_groups;

	fclose(file);
	return ret;
}
