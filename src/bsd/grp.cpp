#include "grp.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"
#include "stdlib.h"

EXPORT int initgroups(const char* user, gid_t group) {
	gid_t* groups = nullptr;
	int prev_count = 0;
	int num_groups = 0;
	while (getgrouplist(user, group, groups, &num_groups) < 0) {
		if (groups) {
			free(groups);
		}

		if (num_groups <= prev_count) {
			return -1;
		}

		if (num_groups < prev_count + prev_count / 2) {
			num_groups = prev_count + prev_count / 2;
		}

		groups = static_cast<gid_t*>(calloc(num_groups, sizeof(gid_t)));
		if (!groups) {
			return -1;
		}
		prev_count = num_groups;
	}

	int ret = setgroups(num_groups, groups);
	free(groups);
	return ret;
}

EXPORT int setgroups(size_t size, const gid_t* list) {
	if (auto err = sys_setgroups(size, list)) {
		errno = err;
		return -1;
	}
	return 0;
}
