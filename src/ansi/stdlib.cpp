#include "stdlib.h"
#include "utils.hpp"
#include "allocator.hpp"
#include "sys.hpp"
#include "unistd.h"
#include "errno.h"
#include "string.h"
#include "strings.h"
#include "ctype.h"
#include "math.h"
#include "wchar.h"
#include "signal.h"
#include "str_to_int.hpp"
#include "str_to_float.hpp"
#include "env.hpp"
#include <hz/bit.hpp>
#include <hz/algorithm.hpp>
#include <hz/vector.hpp>
#include <hz/unordered_map.hpp>
#include <hz/string.hpp>

#define memcpy __builtin_memcpy

hz::vector<char*, Allocator> ENV {Allocator {}};
hz::unordered_map<hz::string<Allocator>, hz::string<Allocator>, Allocator> ALLOCATED_ENV {Allocator {}};

void hzlibc_env_init(char** env) {
	for (; *env; ++env) {
		ENV.push_back(*env);
	}
	ENV.push_back(nullptr);
}

EXPORT void* malloc(size_t size) {
	if (!size) {
		size = 1;
	}
	auto* ptr = allocate(size);
	if (!ptr) {
		errno = ENOMEM;
		return nullptr;
	}
	return ptr;
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
	if (size && num > SIZE_MAX / size) {
		errno = ENOMEM;
		return nullptr;
	}

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

	__ensure(!ENV.empty());
	for (auto* env = ENV.data(); *env; ++env) {
		hz::string_view env_str {*env};
		auto sep = env_str.find('=');
		if (sep == hz::string_view::npos) {
			continue;
		}
		if (env_str.substr(0, sep) == name_str) {
			return *env + sep + 1;
		}
	}

	return nullptr;
}

EXPORT int system(const char* cmd) {
	if (!cmd) {
		return 1;
	}

#if ANSI_ONLY
	if (auto err = sys_system(cmd)) {
		return err;
	}
	return 0;
#else
	struct sigaction new_action {
		.sa_handler = SIG_IGN,
		.sa_mask {},
		.sa_flags = 0,
		.sa_restorer = nullptr
	};
	struct sigaction old_int_action {};
	struct sigaction old_quit_action {};
	sys_sigaction(SIGINT, &new_action, &old_int_action);
	sys_sigaction(SIGQUIT, &new_action, &old_quit_action);

	sigset_t new_mask {};
	sigset_t old_mask {};
	sigaddset(&new_mask, SIGCHLD);
	sys_sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

	int status = -1;
	pid_t child;
	if (auto err = sys_fork(&child)) {
		errno = err;
	}
	else if (!child) {
		sys_sigaction(SIGINT, &old_int_action, nullptr);
		sys_sigaction(SIGQUIT, &old_quit_action, nullptr);
		sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);

		const char* args[] {
			"sh", "-c", cmd, nullptr
		};
		sys_execve("/bin/sh", const_cast<char**>(args), environ);
		_Exit(127);
	}
	else {
		pid_t tmp;
		while ((err = sys_waitpid(child, &status, 0, nullptr, &tmp))) {
			if (err == EINTR) {
				continue;
			}
			errno = err;
			status = -1;
		}
	}

	sys_sigaction(SIGINT, &old_int_action, nullptr);
	sys_sigaction(SIGQUIT, &old_quit_action, nullptr);
	sys_sigprocmask(SIG_SETMASK, &old_mask, nullptr);
	return status;
#endif
}

EXPORT __attribute__((noreturn)) void abort() {
	sys_raise(SIGABRT);
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
	AtExitBlock INITIAL_QUICK_BLOCK {};
	hz::spinlock<AtExitBlock*> BLOCKS {&INITIAL_BLOCK};
	hz::spinlock<AtExitBlock*> QUICK_BLOCKS {&INITIAL_QUICK_BLOCK};

	void run_exit_handlers() {
		auto guard = BLOCKS.lock();
		auto* block = *guard;
		while (block) {
			for (int i = block->size; i > 0; --i) {
				auto& fn = block->fns[i - 1];
				if (!fn.fn) {
					continue;
				}
				fn.fn(fn.arg);
			}

			auto* next = block->next;
			if (block != &INITIAL_BLOCK) {
				delete block;
			}
			block = next;
		}
		*guard = &INITIAL_BLOCK;
		INITIAL_BLOCK.size = 0;
		INITIAL_BLOCK.next = nullptr;
	}
}

extern "C" EXPORT void __cxa_finalize(void*) {}

void call_cxx_tls_destructors();
void __dlapi_destroy();

EXPORT __attribute__((noreturn)) void exit(int status) {
#if !ANSI_ONLY
	call_cxx_tls_destructors();
#endif

	run_exit_handlers();
	__dlapi_destroy();
	sys_exit(status);
}

EXPORT __attribute__((__noreturn__)) void quick_exit(int status) {
	auto guard = QUICK_BLOCKS.lock();
	auto* block = *guard;
	while (block) {
		for (int i = block->size; i > 0; --i) {
			auto& fn = block->fns[i - 1];
			if (!fn.fn) {
				continue;
			}
			fn.fn(fn.arg);
		}

		auto* next = block->next;
		if (block != &INITIAL_QUICK_BLOCK) {
			delete block;
		}
		block = next;
	}
	*guard = &INITIAL_QUICK_BLOCK;
	INITIAL_QUICK_BLOCK.size = 0;
	INITIAL_QUICK_BLOCK.next = nullptr;

	_Exit(status);
}

EXPORT __attribute__((__noreturn__)) void _Exit(int status) {
	__dlapi_destroy();
	sys_exit(status);
}

extern "C" EXPORT int __cxa_atexit(void (*func)(void*), void* arg, void* dso_handle) {
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

EXPORT int at_quick_exit(void (*func)()) {
	auto guard = QUICK_BLOCKS.lock();
	if ((*guard)->size == 8) {
		auto* new_block = new AtExitBlock {};
		new_block->fns[new_block->size++] = {
			.fn = reinterpret_cast<void (*)(void*)>(func),
			.arg = nullptr,
			.dso_handle = nullptr
		};
		new_block->next = *guard;
		*guard = new_block;
	}
	else {
		(*guard)->fns[(*guard)->size++] = {
			.fn = reinterpret_cast<void (*)(void*)>(func),
			.arg = nullptr,
			.dso_handle = nullptr
		};
	}

	return 0;
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

EXPORT long atol(const char* str) {
	bool sign = false;
	if (*str == '-') {
		++str;
		sign = true;
	}
	else if (*str == '+') {
		++str;
	}

	long value = 0;
	for (; isdigit(*str); ++str) {
		value *= 10;
		value += *str - '0';
	}

	if (sign) {
		value *= -1;
	}

	return value;
}

EXPORT long long atoll(const char* str) {
	bool sign = false;
	if (*str == '-') {
		++str;
		sign = true;
	}
	else if (*str == '+') {
		++str;
	}

	long long value = 0;
	for (; isdigit(*str); ++str) {
		value *= 10;
		value += *str - '0';
	}

	if (sign) {
		value *= -1;
	}

	return value;
}

EXPORT double atof(const char* str) {
	return str_to_float<double, char>(str, nullptr);
}

EXPORT long strtol(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	return str_to_int<long, unsigned long>(ptr, end_ptr, base);
}

EXPORT unsigned long strtoul(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	return str_to_int<unsigned long, unsigned long>(ptr, end_ptr, base);
}

EXPORT long long strtoll(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	return str_to_int<long long, unsigned long long>(ptr, end_ptr, base);
}

EXPORT unsigned long long strtoull(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	return str_to_int<unsigned long long, unsigned long long>(ptr, end_ptr, base);
}

EXPORT float strtof(const char* __restrict ptr, char** __restrict end_ptr) {
	return str_to_float<float, char>(ptr, end_ptr);
}

EXPORT double strtod(const char* __restrict ptr, char** __restrict end_ptr) {
	return str_to_float<double, char>(ptr, end_ptr);
}

EXPORT long double strtold(const char* __restrict ptr, char** __restrict end_ptr) {
	return str_to_float<long double, char>(ptr, end_ptr);
}

EXPORT void qsort(void* ptr, size_t count, size_t size, int (*comp)(const void* a, const void* b)) {
	for (size_t i = 0; i < count; ++i) {
		bool changed = false;
		for (size_t j = 0; j < count - 1; ++j) {
			auto* a = reinterpret_cast<char*>(ptr) + j * size;
			auto* b = reinterpret_cast<char*>(ptr) + (j + 1) * size;
			if (comp(a, b) > 0) {
				for (size_t k = 0; k < size; ++k) {
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

EXPORT int mblen(const char* str, size_t len) {
	if (!str) {
		return 0;
	}

	auto size = mbrtowc(nullptr, str, len, nullptr);
	if (size == static_cast<size_t>(-1) ||
		size == static_cast<size_t>(-2)) {
		return -1;
	}
	return *str == 0 ? 0 : static_cast<int>(size);
}

EXPORT size_t mbstowcs(wchar_t* __restrict dest, const char* __restrict src, size_t len) {
	const char* ptr = src;
	return mbsrtowcs(dest, &ptr, len, nullptr);
}

EXPORT int mbtowc(wchar_t* __restrict pwc, const char* __restrict str, size_t len) {
	auto size = mbrtowc(pwc, str, len, nullptr);
	if (size == static_cast<size_t>(-1) ||
	    size == static_cast<size_t>(-2)) {
		return -1;
	}
	return static_cast<int>(size);
}

EXPORT int wctomb(char* str, wchar_t wc) {
	auto size = wcrtomb(str, wc, nullptr);
	if (size == static_cast<size_t>(-1)) {
		return -1;
	}
	return static_cast<int>(size);
}

EXPORT size_t wcstombs(char* dest, const wchar_t* __restrict src, size_t len) {
	if (!dest) {
		len = SIZE_MAX;
	}

	size_t size = 0;
	while (true) {
		char buf[4];
		size_t ret = wcrtomb(buf, *src, nullptr);
		if (ret == static_cast<size_t>(-1)) {
			return ret;
		}
		else if (ret > len) {
			return size;
		}

		if (dest) {
			memcpy(dest, buf, ret);
			dest += ret;
		}

		size += ret;
		len -= ret;

		if (!*src++) {
			break;
		}
	}

	return size - 1;
}

namespace {
	uint32_t RAND_STATE = 1283161862;
}

EXPORT void srand(unsigned int seed) {
	RAND_STATE = seed;
}

EXPORT int rand() {
	uint32_t x = RAND_STATE;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	RAND_STATE = x;
	return static_cast<int>(x % RAND_MAX);
}

EXPORT int abs(int x) {
	return x < 0 ? x * -1 : x;
}

EXPORT long labs(long x) {
	return x < 0 ? x * -1 : x;
}

EXPORT long long llabs(long long x) {
	return x < 0 ? x * -1 : x;
}

EXPORT div_t div(int x, int y) {
	return {
		.quot = x / y,
		.rem = x % y
	};
}

EXPORT ldiv_t ldiv(long x, long y) {
	return {
		.quot = x / y,
		.rem = x % y
	};
}

EXPORT lldiv_t lldiv(long long x, long long y) {
	return {
		.quot = x / y,
		.rem = x % y
	};
}

EXPORT void* bsearch(
	const void* key,
	const void* ptr,
	size_t count,
	size_t size,
	int (*comp)(const void* a, const void* b)) {
	size_t lower = 0;
	size_t upper = count;
	while (lower < upper) {
		size_t middle = (upper - lower) / 2;
		auto* elem = static_cast<const char*>(ptr) + (lower + middle) * size;
		int res = comp(key, elem);
		if (res < 0) {
			upper = lower + middle;
		}
		else if (res > 0) {
			lower = lower + middle + 1;
		}
		else {
			return const_cast<char*>(elem);
		}
	}
	return nullptr;
}

ALIAS(strtol, __isoc23_strtol);
ALIAS(strtoul, __isoc23_strtoul);
ALIAS(strtoull, __isoc23_strtoull);
ALIAS(strtoll, __isoc23_strtoll);
