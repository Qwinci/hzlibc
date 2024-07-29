#include "stdlib.h"
#include "unistd.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "errno.h"
#include "limits.h"
#include "string.h"
#include "sys.hpp"
#include "fcntl.h"
#include <hz/vector.hpp>
#include <hz/string_view.hpp>
#include <hz/string.hpp>
#include <hz/unordered_map.hpp>
#include <hz/algorithm.hpp>

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

EXPORT int posix_memalign(void** ptr, size_t alignment, size_t size) {
	if (!hz::has_single_bit(alignment) || alignment > static_cast<size_t>(sys_getpagesize())) {
		return EINVAL;
	}
	auto* p = Allocator::allocate(hz::max(size, alignment));
	if (!p) {
		return ENOMEM;
	}
	__ensure((reinterpret_cast<uintptr_t>(p) & (alignment - 1)) == 0);
	*ptr = p;
	return 0;
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

EXPORT char* realpath(const char* __restrict path, char* __restrict resolved_path) {
	if (!path || *path == 0) {
		errno = EINVAL;
		return nullptr;
	}

	hz::string<Allocator> resolved {Allocator {}};
	hz::string_view path_str {path};

	if (!path_str.starts_with('/')) {
		char buf[PATH_MAX];
		if (!getcwd(buf, sizeof(buf))) {
			return nullptr;
		}
		resolved += buf;
		if (resolved.as_view().ends_with('/')) {
			resolved.resize(resolved.size() - 1);
		}
	}

	char link_buf[PATH_MAX];
	hz::string<Allocator> link {Allocator {}};

	auto parse_component = [&](hz::string_view component) {
		if (!component.size() || component == "/" || component == ".") {
			return 0;
		}
		else if (component == "..") {
			if (!resolved.empty()) {
				if (auto offset = resolved.as_view().find_last_of("/"); offset != hz::string_view::npos) {
					resolved.resize(offset);
				}
			}
			return 0;
		}

		auto old_resolved_size = resolved.size();
		resolved += '/';
		resolved += component;

		struct stat64 s {};
		if (auto err = sys_stat(StatTarget::Path, 0, resolved.data(), AT_SYMLINK_NOFOLLOW, &s)) {
			return err;
		}

		if (S_ISLNK(s.st_mode)) {
			int link_size;
			if (auto err = sys_readlinkat(AT_FDCWD, resolved.data(), link_buf, sizeof(link_buf), &link_size)) {
				return err;
			}

			if (link[0] == '/') {
				resolved = link;
			}
			else {
				resolved.resize(old_resolved_size);
				link.prepend(link_buf);
			}
		}

		return 0;
	};

	size_t offset = 0;
	while (true) {
		auto end = path_str.find('/', offset);
		auto component = path_str.substr_abs(offset, end);

		if (auto err = parse_component(component)) {
			errno = err;
			return nullptr;
		}

		if (!link.empty()) {
			size_t link_offset = 0;
			while (true) {
				auto link_end = link.as_view().find('/', link_offset);
				auto link_component = link.as_view().substr_abs(link_offset, link_end);

				if (auto err = parse_component(link_component)) {
					errno = err;
					return nullptr;
				}

				if (link_end == hz::string_view::npos) {
					break;
				}
				else {
					link_offset = link_end + 1;
				}
			}

			link.clear();
		}

		if (end == hz::string_view::npos) {
			break;
		}
		else {
			offset = end + 1;
		}
	}

	if (resolved.size() > PATH_MAX - 1) {
		errno = ENAMETOOLONG;
		return nullptr;
	}

	if (!resolved_path) {
		resolved_path = static_cast<char*>(malloc(PATH_MAX));
		if (!resolved_path) {
			return nullptr;
		}
	}

	memcpy(resolved_path, resolved.data(), resolved.size() + 1);

	return resolved_path;
}

EXPORT float strtof_l(const char* __restrict ptr, char** __restrict end_ptr, locale_t locale) {
	println("strtof_l ignores locale");
	return strtof(ptr, end_ptr);
}

EXPORT double strtod_l(const char* __restrict ptr, char** __restrict end_ptr, locale_t locale) {
	println("strtod_l ignores locale");
	return strtod(ptr, end_ptr);
}

EXPORT long double strtold_l(const char* __restrict ptr, char** __restrict end_ptr, locale_t locale) {
	println("strtold_l ignores locale");
	return strtold(ptr, end_ptr);
}

EXPORT int mkstemp(char* template_str) {
	return mkostemp(template_str, 0);
}

// this is glibc but to avoid needing to make the environment vector global its here
EXPORT int clearenv() {
	ENV.clear();
	ENV.shrink_to_fit();
	ENV.push_back(nullptr);
	environ = ENV.data();
	return 0;
}

ALIAS(strtof_l, __strtof_l);
ALIAS(strtod_l, __strtod_l);
ALIAS(mkstemp, mkstemp64);
