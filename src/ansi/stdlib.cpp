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
#include <hz/bit.hpp>
#include <hz/algorithm.hpp>

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

	__ensure(environ);
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

EXPORT int system(const char* cmd) {
	if (!cmd) {
		return 1;
	}

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
	AtExitBlock INITIAL_QUICK_BLOCK {};
	hz::spinlock<AtExitBlock*> BLOCKS {&INITIAL_BLOCK};
	hz::spinlock<AtExitBlock*> QUICK_BLOCKS {&INITIAL_QUICK_BLOCK};
}

extern "C" EXPORT void __cxa_finalize(void* dso_handle) {
	auto guard = BLOCKS.lock();
	auto* block = *guard;
	if (!dso_handle) {
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
	else {
		AtExitBlock* prev = nullptr;
		while (block) {
			int count_called = 0;
			for (int i = block->size; i > 0; --i) {
				auto& fn = block->fns[i - 1];
				if (fn.dso_handle == dso_handle && fn.fn) {
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
}

void call_cxx_tls_destructors();
void __dlapi_exit();

EXPORT __attribute__((noreturn)) void exit(int status) {
	call_cxx_tls_destructors();
	__cxa_finalize(nullptr);
	__dlapi_exit();
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

template<typename T, typename U>
static T str_to_int(const char* __restrict ptr, char** __restrict end_ptr, int base) {
	while (isspace(*ptr)) {
		++ptr;
	}

	bool sign = false;
	if (*ptr == '-') {
		++ptr;
		sign = true;
	}
	else if (*ptr == '+') {
		++ptr;
	}

	if (base == 0) {
		if (ptr[0] == '0' && tolower(ptr[1]) == 'x' && (ptr[2] >= '0' && tolower(ptr[2]) <= CHARS[16 - 1])) {
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

	U max_value;
	if constexpr (hz::is_signed_v<T>) {
		if (sign) {
			max_value = static_cast<U>(-(hz::numeric_limits<T>::min() + 1)) + 1;
		}
		else {
			max_value = static_cast<U>(hz::numeric_limits<T>::max());
		}
	}
	else {
		max_value = static_cast<U>(hz::numeric_limits<T>::max());
	}

	U value = 0;
	bool overflow = false;
	if (base <= 36) {
		for (; *ptr >= '0' && tolower(*ptr) <= CHARS[base - 1]; ++ptr) {
			auto old = value;
			value *= base;
			if (value / base != old || value > max_value) {
				overflow = true;
			}
			old = value;
			value += *ptr <= '9' ? (*ptr - '0') : (tolower(*ptr) - 'a' + 10);
			if (value < old || value > max_value) {
				overflow = true;
			}
		}
	}

	if (end_ptr) {
		*end_ptr = const_cast<char*>(ptr);
	}

	if (overflow) {
		errno = ERANGE;
		return sign ? hz::numeric_limits<T>::min() : hz::numeric_limits<T>::max();
	}

	if (sign) {
		return -static_cast<T>(value);
	}
	else {
		return static_cast<T>(value);
	}
}

template<typename T>
T str_to_float(const char* __restrict ptr, char** __restrict end_ptr) {
	while (isspace(*ptr)) {
		++ptr;
	}

	bool sign = false;
	if (*ptr == '-') {
		++ptr;
		sign = true;
	}
	else if (*ptr == '+') {
		++ptr;
	}

	if (strncasecmp(ptr, "nan", 3) == 0) {
		ptr += 3;
		if (*ptr == '(') {
			while (*ptr != ')') {
				++ptr;
			}
			++ptr;
		}

		if (end_ptr) {
			*end_ptr = const_cast<char*>(ptr);
		}

		if constexpr (sizeof(T) == 4) {
			return __builtin_nanf("");
		}
		else if constexpr (sizeof(T) == 8) {
			return __builtin_nan("");
		}
		else {
			return __builtin_nanl("");
		}
	}
	else {
		bool inf = false;
		if (strncasecmp(ptr, "inf", 3) == 0) {
			ptr += 3;
			inf = true;
		}
		else if (strncasecmp(ptr, "infinity", 8) == 0) {
			ptr += 8;
			inf = true;
		}

		if (inf) {
			if (end_ptr) {
				*end_ptr = const_cast<char*>(ptr);
			}

			if constexpr (sizeof(T) == 4) {
				return __builtin_inff();
			}
			else if constexpr (sizeof(T) == 8) {
				return __builtin_inf();
			}
			else {
				return __builtin_infl();
			}
		}
	}

	int base = 10;
	if (*ptr == '0' && tolower(ptr[1]) == 'x') {
		ptr += 2;
		base = 16;
	}

	T value {};
	while (*ptr >= '0' && *ptr <= CHARS[base - 1]) {
		auto c = *ptr++;
		auto digit = c <= '9' ? (c - '0') : (tolower(c) - 'a' + 10);
		value *= base;
		value += digit;
	}

	if (*ptr == '.') {
		++ptr;

		T decimal {1};
		while (*ptr >= '0' && *ptr <= CHARS[base - 1]) {
			auto c = *ptr++;
			auto digit = c <= '9' ? (c - '0') : (tolower(c) - 'a' + 10);
			decimal /= base;
			value += decimal * digit;
		}
	}

	char exponent_char;
	if (base == 10) {
		exponent_char = 'e';
	}
	else {
		exponent_char = 'p';
	}

	if (tolower(*ptr) == exponent_char) {
		++ptr;

		bool exponent_sign = false;
		if (*ptr == '-') {
			++ptr;
			exponent_sign = true;
		}
		else if (*ptr == '+') {
			++ptr;
		}

		int exponent = 0;
		while (*ptr >= '0' && *ptr <= '9') {
			auto digit = *ptr++ - '0';
			exponent *= 10;
			exponent += digit;
		}

		int exponent_value = base == 10 ? 10 : 2;
		if (exponent_sign) {
			for (int i = 0; i < exponent; ++i) {
				value /= exponent_value;
			}
		}
		else {
			for (int i = 0; i < exponent; ++i) {
				value *= exponent_value;
			}
		}
	}

	if (isinf(value)) {
		errno = ERANGE;
	}

	if (end_ptr) {
		*end_ptr = const_cast<char*>(ptr);
	}

	return sign ? -value : value;
}

EXPORT double atof(const char* str) {
	return str_to_float<double>(str, nullptr);
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
	return str_to_float<float>(ptr, end_ptr);
}

EXPORT double strtod(const char* __restrict ptr, char** __restrict end_ptr) {
	return str_to_float<double>(ptr, end_ptr);
}

EXPORT long double strtold(const char* __restrict ptr, char** __restrict end_ptr) {
	return str_to_float<long double>(ptr, end_ptr);
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
	return *str == 0 ? 0 : static_cast<int>(size);
}

EXPORT int wctomb(char* str, wchar_t wc) {
	auto size = wcrtomb(str, wc, nullptr);
	if (size == static_cast<size_t>(-1)) {
		return -1;
	}
	return static_cast<int>(size);
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
