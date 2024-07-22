#include "stdlib.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "sys.hpp"
#include "unistd.h"
#include "errno.h"
#include "string.h"
#include "ctype.h"
#include "limits.h"
#include <hz/bit.hpp>
#include <hz/algorithm.hpp>

EXPORT void* malloc(size_t size) {
	if (!size) {
		size = 1;
	}
	return allocate(size);
}

EXPORT void* realloc(void* old, size_t new_size) {
	if (!new_size) {
		new_size = 1;
	}
	auto ptr = reallocate(old, new_size);
	if (!ptr) {
		errno = ENOMEM;
	}
	return ptr;
}

EXPORT void* aligned_alloc(size_t alignment, size_t size) {
	if (alignment <= 16) {
		return malloc(size);
	}
	else if (!hz::has_single_bit(alignment) || alignment > static_cast<size_t>(sys_getpagesize())) {
		errno = EINVAL;
		return nullptr;
	}
	void* ptr = malloc(hz::max(size, alignment));
	if (!ptr) {
		return nullptr;
	}
	__ensure((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0);
	return ptr;
}

EXPORT void* calloc(size_t num, size_t size) {
	if (!size || !num) {
		num = 1;
		size = 1;
	}
	size_t total = num * size;
	auto ptr = allocate(total);
	if (ptr) {
		memset(ptr, 0, total);
	}
	else {
		errno = ENOMEM;
	}
	return ptr;
}

EXPORT void free(void* ptr) {
	deallocate(ptr);
}

EXPORT char* getenv(const char* name) {
	hz::string_view name_str {name};

	for (auto* env = environ; *env; ++env) {
		hz::string_view env_str {*env};
		auto sep = env_str.find('=');
		__ensure(sep != hz::string_view::npos);
		if (env_str.substr(0, sep) == name_str) {
			return *env + sep + 1;
		}
	}

	return nullptr;
}

EXPORT __attribute__((noreturn)) void abort() {
	sys_kill(sys_get_process_id(), SIGABRT);
	sys_exit(1);
}

namespace {
	using AtExitFn = void (*)(void*);

	struct AtExit {
		AtExitFn fn;
		void* arg;
		void* dso_handle;
	};

	struct AtExitBlock {
		AtExitBlock* next;
		AtExit fns[8];
		int size;
	};

	AtExitBlock INITIAL_BLOCK {};
	hz::spinlock<AtExitBlock*> BLOCKS {&INITIAL_BLOCK};
}

extern "C" EXPORT void __cxa_finalize(void* dso_handle) {
	auto guard = BLOCKS.lock();
	auto* block = *guard;
	AtExitBlock* prev = nullptr;
	while (block) {
		int count_called = 0;
		for (int i = block->size; i > 0; --i) {
			auto& fn = block->fns[i];
			if (fn.dso_handle == dso_handle) {
				fn.fn(fn.arg);
				fn.fn = nullptr;
				++count_called;
			}
		}

		auto* next = block->next;
		if (block != &INITIAL_BLOCK && count_called == block->size) {
			delete block;
			if (prev) {
				prev->next = next;
			}
			else {
				*guard = next;
			}
		}
		else {
			prev = block;
		}
		block = next;
	}
}

void __dlapi_exit();

EXPORT __attribute__((noreturn)) void exit(int status) {
	{
		auto guard = BLOCKS.lock();
		auto* block = *guard;
		AtExitBlock* prev = nullptr;
		while (block) {
			int count_called = 0;
			for (int i = block->size; i > 0; --i) {
				auto& fn = block->fns[i - 1];
				if (fn.dso_handle || !fn.fn) {
					continue;
				}
				fn.fn(fn.arg);
				fn.fn = nullptr;
				++count_called;
			}

			auto* next = block->next;
			if (block != &INITIAL_BLOCK && count_called == block->size) {
				delete block;
				if (prev) {
					prev->next = next;
				}
				else {
					*guard = next;
				}
			}
			else {
				prev = block;
			}
			block = next;
		}
	}

	__dlapi_exit();
	sys_exit(status);
}

extern "C" EXPORT int __cxa_atexit(void (*func)(void*), void* arg, [[maybe_unused]] void* dso_handle) {
	auto guard = BLOCKS.lock();
	if ((*guard)->size == 8) {
		auto* new_block = new AtExitBlock {};
		new_block->fns[new_block->size++] = {
			.fn = func,
			.arg = arg,
			.dso_handle = dso_handle
		};
		new_block->next = *guard;
		*guard = new_block;
	}
	else {
		(*guard)->fns[(*guard)->size++] = {
			.fn = func,
			.arg = arg,
			.dso_handle = dso_handle
		};
	}

	return 0;
}

EXPORT int atexit(void (*func)()) {
	return __cxa_atexit(reinterpret_cast<AtExitFn>(func), nullptr, nullptr);
}

namespace {
	constexpr char CHARS[] = "0123456789abcdefghijklmnopqrstuvwxyz";
}

EXPORT int atoi(const char* str) {
	bool sign = false;
	if (*str == '-') {
		++str;
		sign = true;
	}
	else if (*str == '+') {
		++str;
	}

	int value = 0;
	for (; isdigit(*str); ++str) {
		value *= 10;
		value += *str - '0';
	}

	if (sign) {
		value *= -1;
	}

	return value;
}

EXPORT unsigned long strtoul(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	bool sign = false;
	if (*ptr == '-') {
		++ptr;
		sign = true;
	}
	else if (*ptr == '+') {
		++ptr;
	}

	if (base == 0) {
		if (ptr[0] == '0' && tolower(ptr[1]) == 'x') {
			base = 16;
			ptr += 2;
		}
		else if (ptr[0] == '0') {
			base = 8;
			++ptr;
		}
		else {
			base = 10;
		}
	}
	else if (base == 8) {
		if (*ptr == '0') {
			++ptr;
		}
	}
	else if (base == 16) {
		if (ptr[0] == '0' && tolower(ptr[1]) == 'x') {
			ptr += 2;
		}
	}

	unsigned long value = 0;
	if (base <= 36) {
		for (; *ptr >= '0' && tolower(*ptr) <= CHARS[base - 1]; ++ptr) {
			auto old = value;
			value *= base;
			if (value / base != old) {
				errno = ERANGE;
				return ULONG_MAX;
			}
			old = value;
			value += *ptr <= '9' ? (*ptr - '0') : (tolower(*ptr) - 'a' + 10);
			if (value < old) {
				errno = ERANGE;
				return ULONG_MAX;
			}
		}
	}

	if (end_ptr) {
		*end_ptr = const_cast<char*>(ptr);
	}
	if (sign) {
		value = -value;
	}
	return value;
}

EXPORT void qsort(void* ptr, size_t count, size_t size, int (*comp)(const void* a, const void* b)) {
	for (size_t i = 0; i < count; ++i) {
		bool changed = false;
		for (size_t j = 0; j < count - 1; ++j) {
			auto* a = reinterpret_cast<char*>(ptr) + j * size;
			auto* b = reinterpret_cast<char*>(ptr) + (j + 1) * size;
			if (comp(a, b) > 0) {
				for (size_t k = 0; k < count; ++k) {
					char tmp = a[k];
					a[k] = b[k];
					b[k] = tmp;
				}
				changed = true;
			}
		}

		if (!changed) {
			break;
		}
	}
}

ALIAS(strtoul, __isoc23_strtoul);
