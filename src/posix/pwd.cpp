#include "pwd.h"
#include "utils.hpp"
#include "stdio.h"
#include "errno.h"
#include "allocator.hpp"
#include <hz/string_utils.hpp>
#include <hz/string.hpp>

namespace {
	hz::string<Allocator> PWD_STR_AREA {Allocator {}};
	FILE* GLOBAL_FILE;

	struct user_view {
		hz::string_view name;
		hz::string_view password;
		uid_t uid {};
		gid_t gid {};
		hz::string_view info;
		hz::string_view dir;
		hz::string_view shell;
	};

	bool parse_entry(hz::string_view line, user_view& view) {
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
		hz::string_view user_id;
		if (!parse_segment(user_id)) {
			return false;
		}
		hz::string_view group_id;
		if (!parse_segment(group_id)) {
			return false;
		}
		hz::string_view info;
		if (!parse_segment(info)) {
			return false;
		}
		hz::string_view home;
		if (!parse_segment(home)) {
			return false;
		}
		hz::string_view shell;
		if (parse_segment(shell)) {
			return false;
		}

		size_t count;
		auto uid = hz::to_integer<uid_t>(user_id, count);
		if (count != user_id.size()) {
			return false;
		}

		auto gid = hz::to_integer<gid_t>(group_id, count);
		if (count != group_id.size()) {
			return false;
		}

		view.name = name;
		view.password = password;
		view.uid = uid;
		view.gid = gid;
		view.info = info;
		view.dir = home;
		view.shell = shell;
		return true;
	}

	void fill_user(user_view view, passwd& res) {
		PWD_STR_AREA = view.name;
		PWD_STR_AREA.push_back(0);
		auto pwd_passwd_offset = PWD_STR_AREA.size();
		PWD_STR_AREA += view.password;
		PWD_STR_AREA.push_back(0);
		auto pwd_gecos_offset = PWD_STR_AREA.size();
		PWD_STR_AREA += view.info;
		PWD_STR_AREA.push_back(0);
		auto pwd_dir_offset = PWD_STR_AREA.size();
		PWD_STR_AREA += view.dir;
		PWD_STR_AREA.push_back(0);
		auto pwd_shell_offset = PWD_STR_AREA.size();
		PWD_STR_AREA += view.shell;
		PWD_STR_AREA.push_back(0);

		res.pw_name = PWD_STR_AREA.data();
		res.pw_passwd = PWD_STR_AREA.data() + pwd_passwd_offset;
		res.pw_uid = view.uid;
		res.pw_gid = view.gid;
		res.pw_gecos = PWD_STR_AREA.data() + pwd_gecos_offset;
		res.pw_dir = PWD_STR_AREA.data() + pwd_dir_offset;
		res.pw_shell = PWD_STR_AREA.data() + pwd_shell_offset;
	}

	bool open_global_file() {
		if (!GLOBAL_FILE) {
			GLOBAL_FILE = fopen("/etc/passwd", "r");
			if (!GLOBAL_FILE) {
				errno = EIO;
				return false;
			}
		}

		return true;
	}
}

EXPORT passwd* getpwnam(const char* name) {
	static passwd pwd;

	FILE* file = fopen("/etc/passwd", "r");
	if (!file) {
		errno = EIO;
		return nullptr;
	}

	hz::string_view name_str {name};

	char buf[NSS_BUFLEN_PASSWD];
	while (fgets(buf, NSS_BUFLEN_PASSWD, file)) {
		user_view view;
		if (!parse_entry(buf, view)) {
			continue;
		}
		if (view.name == name_str) {
			fill_user(view, pwd);
			return &pwd;
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

EXPORT passwd* getpwuid(uid_t uid) {
	static passwd pwd;

	FILE* file = fopen("/etc/passwd", "r");
	if (!file) {
		errno = EIO;
		return nullptr;
	}

	char buf[NSS_BUFLEN_PASSWD];
	while (fgets(buf, NSS_BUFLEN_PASSWD, file)) {
		user_view view;
		if (!parse_entry(buf, view)) {
			continue;
		}
		if (view.uid == uid) {
			fill_user(view, pwd);
			return &pwd;
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

EXPORT void setpwent() {
	if (!open_global_file()) {
		return;
	}
	rewind(GLOBAL_FILE);
}

EXPORT passwd* getpwent() {
	static passwd pwd;

	if (!open_global_file()){
		return nullptr;
	}

	char buf[NSS_BUFLEN_PASSWD];
	while (fgets(buf, NSS_BUFLEN_PASSWD, GLOBAL_FILE)) {
		user_view view;
		if (!parse_entry(buf, view)) {
			continue;
		}
		fill_user(view, pwd);
		return &pwd;
	}

	errno = ENOENT;
	return nullptr;
}

EXPORT void endpwent() {
	if (GLOBAL_FILE) {
		if (fclose(GLOBAL_FILE) != 0) {
			errno = EIO;
		}
		GLOBAL_FILE = nullptr;
	}
}
