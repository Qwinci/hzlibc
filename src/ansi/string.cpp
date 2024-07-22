#include "string.h"
#include "utils.hpp"
#include "stdlib.h"
#include "errno.h"
#include <hz/array.hpp>

EXPORT size_t strlen(const char* str) {
	size_t len = 0;
	while (*str++) ++len;
	return len;
}

EXPORT int strcmp(const char* lhs, const char* rhs) {
	for (;; ++lhs, ++rhs) {
		int res = *lhs - *rhs;
		if (res != 0 || !*lhs) {
			return res;
		}
	}
}

EXPORT int strncmp(const char* lhs, const char* rhs, size_t count) {
	for (; count; --count, ++lhs, ++rhs) {
		int res = *lhs - *rhs;
		if (res != 0 || !*lhs) {
			return res;
		}
	}
	return 0;
}

EXPORT char* strchr(const char* str, int ch) {
	for (; *str; ++str) {
		if (*str == ch) {
			return const_cast<char*>(str);
		}
	}

	return ch == 0 ? const_cast<char*>(str) : nullptr;
}

EXPORT char* strrchr(const char* str, int ch) {
	auto size = strlen(str) + 1;
	for (size_t i = size; i > 0; --i) {
		if (str[i - 1] == ch) {
			return const_cast<char*>(&str[i - 1]);
		}
	}

	return nullptr;
}

EXPORT char* strcpy(char* __restrict dest, const char* __restrict src) {
	char* orig_dest = dest;
	for (; *src;) {
		*dest++ = *src++;
	}
	*dest = 0;
	return orig_dest;
}

EXPORT char* strncpy(char* __restrict dest, const char* __restrict src, size_t count) {
	char* orig_dest = dest;
	for (; count && *src; --count) {
		*dest++ = *src++;
	}
	if (count) {
		*dest = 0;
	}
	return orig_dest;
}

EXPORT char* strdup(const char* str) {
	size_t len = strlen(str);
	char* mem = static_cast<char*>(malloc(len + 1));
	if (!mem) {
		return nullptr;
	}
	memcpy(mem, str, len + 1);
	return mem;
}

EXPORT size_t strspn(const char* str, const char* search) {
	size_t len = strlen(search);
	size_t found = 0;
	for (; *str; ++str) {
		bool search_found = false;
		for (size_t i = 0; i < len; ++i) {
			if (*str == search[i]) {
				search_found = true;
				++found;
				break;
			}
		}

		if (!search_found) {
			break;
		}
	}

	return found;
}

EXPORT size_t strcspn(const char* str, const char* search) {
	size_t len = strlen(search);
	size_t found = 0;
	for (; *str; ++str, ++found) {
		for (size_t i = 0; i < len; ++i) {
			if (*str == search[i]) {
				return found;
			}
		}
	}

	return found;
}

EXPORT int strcoll(const char* lhs, const char* rhs) {
	println("strcoll ignores locale");
	return strcmp(lhs, rhs);
}

namespace {
	constexpr auto get_errnos() {
		hz::array<const char*, EHWPOISON + 1> array {};
		for (int i = 0; i < EHWPOISON; ++i) {
			array[i] = "Unknown error";
		}

		array[EPERM] = "Operation not permitted";
		array[ENOENT] = "No such file or directory";
		array[ESRCH] = "No such process";
		array[EINTR] = "Interrupted function call";
		array[EIO] = "Input/output error";
		array[EAGAIN] = "Resource temporarily unavailable";
		array[ENOMEM] = "Not enough space";
		array[EACCES] = "Permission denied";
		array[EINVAL] = "Invalid argument";
		array[ENOTTY] = "Inappropriate I/O control operation";
		array[ERANGE] = "Result too large";
		array[EDEADLK] = "Resource deadlock avoided";
		array[ENAMETOOLONG] = "Filename too long";
		array[ENOSYS] = "Function not implemented";
		array[EOVERFLOW] = "Value too large to be stored in data type";
		array[EILSEQ] = "Invalid or incomplete multibyte or wide character";
		array[EHWPOISON] = "Memory page has hardware error";

		return array;
	}

	constexpr auto ERRNOS = get_errnos();
}

EXPORT char* strerror(int err_num) {
	if (err_num < static_cast<int>(ERRNOS.size())) {
		return const_cast<char*>(ERRNOS[err_num]);
	}
	else {
		errno = EINVAL;
		return nullptr;
	}
}

#if defined(__x86_64__) && defined(OPTIMIZED_ASM)

EXPORT void* memset(void* __restrict dest, int ch, size_t size) {
	void* dest_copy = dest;
	asm volatile("rep stosb" : "+D"(dest_copy), "+c"(size) : "a"(ch) : "flags", "memory");
	return dest;
}

EXPORT void* memcpy(void* __restrict dest, const void* __restrict src, size_t size) {
	auto* dest_ptr = static_cast<unsigned char*>(dest);
	auto* src_ptr = static_cast<const unsigned char*>(src);

	if (reinterpret_cast<uintptr_t>(dest_ptr) % 8 != reinterpret_cast<uintptr_t>(src_ptr) % 8) {
		goto slow;
	}
	else if (size < 16) {
		goto fast_8;
	}

	while (reinterpret_cast<uintptr_t>(dest_ptr) % 8 ||
		   reinterpret_cast<uintptr_t>(src_ptr) % 8) {
		*dest_ptr++ = *src_ptr++;
		--size;
	}

	for (; size >= 64; size -= 64) {
		auto value0 = *reinterpret_cast<const uint64_t*>(src_ptr);
		auto value1 = *reinterpret_cast<const uint64_t*>(src_ptr + 8);
		auto value2 = *reinterpret_cast<const uint64_t*>(src_ptr + 16);
		auto value3 = *reinterpret_cast<const uint64_t*>(src_ptr + 24);
		auto value4 = *reinterpret_cast<const uint64_t*>(src_ptr + 32);
		auto value5 = *reinterpret_cast<const uint64_t*>(src_ptr + 40);
		auto value6 = *reinterpret_cast<const uint64_t*>(src_ptr + 48);
		auto value7 = *reinterpret_cast<const uint64_t*>(src_ptr + 56);
		asm volatile(
			"movnti %0, %1;"
			"movnti %2, %3;"
			"movnti %4, %5;"
			"movnti %6, %7;"

			"movnti %8, %9;"
			"movnti %10, %11;"
			"movnti %12, %13;"
			"movnti %14, %15" : :
			"r"(value0), "m"(*dest_ptr),
			"r"(value1), "m"(*(dest_ptr + 8)),
			"r"(value2), "m"(*(dest_ptr + 16)),
			"r"(value3), "m"(*(dest_ptr + 24)),

			"r"(value4), "m"(*(dest_ptr + 32)),
			"r"(value5), "m"(*(dest_ptr + 40)),
			"r"(value6), "m"(*(dest_ptr + 48)),
			"r"(value7), "m"(*(dest_ptr + 56)));
		src_ptr += 64;
		dest_ptr += 64;
	}

	for (; size >= 32; size -= 32) {
		auto value0 = *reinterpret_cast<const uint64_t*>(src_ptr);
		auto value1 = *reinterpret_cast<const uint64_t*>(src_ptr + 8);
		auto value2 = *reinterpret_cast<const uint64_t*>(src_ptr + 16);
		auto value3 = *reinterpret_cast<const uint64_t*>(src_ptr + 24);
		asm volatile(
			"movnti %0, %1;"
			"movnti %2, %3;"
			"movnti %4, %5;"
			"movnti %6, %7;" : :
			"r"(value0), "m"(*dest_ptr),
			"r"(value1), "m"(*(dest_ptr + 8)),
			"r"(value2), "m"(*(dest_ptr + 16)),
			"r"(value3), "m"(*(dest_ptr + 24)));
		src_ptr += 32;
		dest_ptr += 32;
	}

fast_8:
	for (; size >= 8; size -= 8) {
		auto value0 = *reinterpret_cast<const uint64_t*>(src_ptr);
		asm volatile(
			"movnti %0, %1;": :
			"r"(value0), "m"(*dest_ptr));
		src_ptr += 8;
		dest_ptr += 8;
	}

slow:
	for (; size; --size) {
		*dest_ptr++ = *src_ptr++;
	}

	return dest;
}

#else

EXPORT void* memset(void* dest, int ch, size_t size) {
	auto* ptr = static_cast<unsigned char*>(dest);
	for (; size; --size) {
		*ptr++ = static_cast<unsigned char>(ch);
	}
	return dest;
}

EXPORT void* memcpy(void* __restrict dest, const void* __restrict src, size_t size) {
	auto* dest_ptr = static_cast<unsigned char*>(dest);
	auto* src_ptr = static_cast<const unsigned char*>(src);
	for (; size; --size) {
		*dest_ptr++ = *src_ptr++;
	}
	return dest;
}

#endif

EXPORT int memcmp(const void* lhs, const void* rhs, size_t count) {
	auto* lhs_ptr = static_cast<const unsigned char*>(lhs);
	auto* rhs_ptr = static_cast<const unsigned char*>(rhs);
	for (; count; --count, ++lhs_ptr, ++rhs_ptr) {
		int res = *lhs_ptr - *rhs_ptr;
		if (res != 0) {
			return res;
		}
	}
	return 0;
}

EXPORT void* memchr(const void* ptr, int ch, size_t count) {
	auto* p = static_cast<const unsigned char*>(ptr);
	for (; count; ++p, --count) {
		if (*p == static_cast<unsigned char>(ch)) {
			return const_cast<unsigned char*>(p);
		}
	}
	return nullptr;
}

EXPORT void* memmove(void* dest, const void* src, size_t size) {
	auto dest_addr = reinterpret_cast<uintptr_t>(dest);
	auto src_addr = reinterpret_cast<uintptr_t>(src);

	if (dest_addr > src_addr && dest_addr - src_addr < size) {
		auto* dest_ptr = static_cast<unsigned char*>(dest);
		auto* src_ptr = static_cast<const unsigned char*>(src);
		for (size_t i = size; i > 0; --i) {
			dest_ptr[i - 1] = src_ptr[i - 1];
		}
		return dest;
	}
	else {
		return memcpy(dest, src, size);
	}
}
