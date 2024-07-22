#include "stdlib.h"
#include "unistd.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "string.h"
#include "errno.h"
#include <hz/vector.hpp>
#include <hz/string_view.hpp>
#include <hz/string.hpp>
#include <hz/unordered_map.hpp>

namespace {
	hz::vector<char*, Allocator> ENV {Allocator {}};
	hz::unordered_map<hz::string<Allocator>, hz::string<Allocator>, Allocator> ALLOCATED_ENV {Allocator {}};
}

void hzlibc_env_init(char** env) {
	for (; *env; ++env) {
		ENV.push_back(*env);
	}
	ENV.push_back(nullptr);
	environ = ENV.data();
}

EXPORT int setenv(const char* name, const char* value, int overwrite) {
	if (!name || !*name) {
		errno = EINVAL;
		return -1;
	}

	hz::string_view name_str {name};
	if (name_str.contains('=')) {
		errno = EINVAL;
		return -1;
	}

	for (size_t i = 0; i < ENV.size() - 1; ++i) {
		hz::string_view str {ENV[i]};
		size_t name_end = str.find('=');
		if (name_end == hz::string_view::npos) {
			panic("setenv: environment contains string without '='");
		}
		auto var_name = str.substr(0, name_end);

		if (var_name == name_str) {
			if (overwrite) {
				hz::string<Allocator> new_value {Allocator {}};
				new_value = name_str;
				new_value += '=';
				new_value += value;

				hz::string<Allocator> alloc_name {Allocator {}};
				alloc_name = name;

				auto new_value_ptr = new_value.data();
				ALLOCATED_ENV.insert(std::move(alloc_name), std::move(new_value));

				ENV[i] = new_value_ptr;
			}
			return 0;
		}
	}

	hz::string<Allocator> new_value {Allocator {}};
	new_value = name_str;
	new_value += '=';
	new_value += value;

	hz::string<Allocator> alloc_name {Allocator {}};
	alloc_name = name;

	auto new_value_ptr = new_value.data();
	ALLOCATED_ENV.insert(std::move(alloc_name), std::move(new_value));

	ENV.back() = new_value_ptr;
	ENV.push_back(nullptr);
	environ = ENV.data();
	return 0;
}

EXPORT int unsetenv(const char* name) {
	if (!name || !*name) {
		errno = EINVAL;
		return -1;
	}

	hz::string_view name_str {name};
	if (name_str.contains('=')) {
		errno = EINVAL;
		return -1;
	}

	for (size_t i = 0; i < ENV.size() - 1; ++i) {
		hz::string_view str {ENV[i]};
		size_t name_end = str.find('=');
		if (name_end == hz::string_view::npos) {
			panic("unsetenv: environment contains string without '='");
		}
		auto var_name = str.substr(0, name_end);

		if (var_name == name_str) {
			ALLOCATED_ENV.remove(var_name);
			auto old_last = ENV[ENV.size() - 2];
			ENV[i] = old_last;
			ENV.pop_back();
			ENV.back() = nullptr;
			environ = ENV.data();
			break;
		}
	}

	return 0;
}

// this is glibc but to avoid needing to make the environment vector global its here
EXPORT int clearenv() {
	ENV.clear();
	ENV.shrink_to_fit();
	ENV.push_back(nullptr);
	environ = ENV.data();
	return 0;
}
