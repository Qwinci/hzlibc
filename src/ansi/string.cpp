#include "string.h"
#include "utils.hpp"
#include "stdlib.h"
#include "errno.h"
#include "string.hpp"
#include <hz/array.hpp>
#include <hz/algorithm.hpp>

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

EXPORT char* strstr(const char* str, const char* substr) {
	hz::string_view a {str};
	if (auto pos = a.find(substr); pos != hz::string_view::npos) {
		return const_cast<char*>(str + pos);
	}
	return nullptr;
}

EXPORT char* strpbrk(const char* str, const char* break_set) {
	hz::string_view str_view {str};
	if (auto pos = str_view.find_first_of(break_set); pos != hz::string_view::npos) {
		return const_cast<char*>(str + pos);
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

EXPORT char* strcat(char* __restrict dest, const char* __restrict src) {
	size_t len = strlen(dest);
	size_t src_len = strlen(src);
	memcpy(dest + len, src, src_len + 1);
	return dest;
}

EXPORT void* memccpy(void* __restrict dest, const void* __restrict src, int ch, size_t size) {
	auto* dest_ptr = static_cast<unsigned char*>(dest);
	auto* src_ptr = static_cast<const unsigned char*>(src);
	auto c = static_cast<unsigned char>(ch);
	for (; size; --size, ++dest_ptr, ++src_ptr) {
		*dest_ptr = *src_ptr;
		if (*dest_ptr == c) {
			return dest_ptr + 1;
		}
	}
	return nullptr;
}

EXPORT char* strncat(char* __restrict dest, const char* __restrict src, size_t count) {
	size_t len = strlen(dest);
	size_t src_len = strlen(src);
	auto to_write = hz::max(src_len, count);
	memcpy(dest + len, src, to_write);
	dest[len + to_write] = 0;
	return dest;
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

EXPORT char* strndup(const char* str, size_t size) {
	size_t len = 0;
	for (auto* ptr = str; *ptr && size; --size, ++ptr) {
		++len;
	}

	char* mem = static_cast<char*>(malloc(len + 1));
	if (!mem) {
		return nullptr;
	}
	memcpy(mem, str, len);
	mem[len] = 0;
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

namespace {
	char* STRTOK_SAVE_PTR = nullptr;
}

EXPORT char* strtok(char* __restrict str, const char* __restrict delim) {
	return internal::strtok_r(str, delim, &STRTOK_SAVE_PTR);
}

EXPORT int strcoll(const char* lhs, const char* rhs) {
	//println("strcoll ignores locale");
	return strcmp(lhs, rhs);
}

EXPORT size_t strxfrm(char* __restrict dest, const char* __restrict src, size_t count) {
	//println("strxfrm ignores locale");
	size_t len = strlen(src);
	if (dest && count) {
		if (len + 1 < count) {
			memcpy(dest, src, len + 1);
		}
		else {
			memcpy(dest, src, count - 1);
			dest[count - 1] = 0;
		}
	}
	return len;
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
		array[ENXIO] = "No such device or address";
		array[E2BIG] = "Argument list too long";
		array[ENOEXEC] = "Exec format error";
		array[EBADF] = "Bad file descriptor";
		array[ECHILD] = "No child processes";
		array[EAGAIN] = "Resource temporarily unavailable";
		array[ENOMEM] = "Not enough space";
		array[EACCES] = "Permission denied";
		array[EFAULT] = "Bad address";
		array[ENOTBLK] = "Block device required";
		array[EBUSY] = "Device or resource busy";
		array[EEXIST] = "File exists";
		array[EXDEV] = "Cross-device link";
		array[ENODEV] = "No such device";
		array[ENOTDIR] = "Not a directory";
		array[EISDIR] = "Is a directory";
		array[EINVAL] = "Invalid argument";
		array[ENFILE] = "File table overflow";
		array[EMFILE] = "Too many open files";
		array[ENOTTY] = "Inappropriate I/O control operation";
		array[ETXTBSY] = "Text file busy";
		array[EFBIG] = "File too large";
		array[ENOSPC] = "No space left on device";
		array[ESPIPE] = "Illegal seek";
		array[EROFS] = "Read-only file system";
		array[EMLINK] = "Too many links";
		array[EPIPE] = "Broken pipe";
		array[EDOM] = "Math argument out of domain of func";
		array[ERANGE] = "Result too large";
		array[EDEADLK] = "Resource deadlock avoided";
		array[ENAMETOOLONG] = "Filename too long";
		array[ENOLCK] = "No record locks available";
		array[ENOSYS] = "Function not implemented";
		array[ENOTEMPTY] = "Directory not empty";
		array[ELOOP] = "Too many symbolic links encountered";
		array[ENOMSG] = "No message of desired type";
		array[EIDRM] = "Identifier removed";
		array[ECHRNG] = "Channels number out of range";
		array[EL2NSYNC] = "Level 2 not synchronized";
		array[EL3HLT] = "Level 3 halted";
		array[EL3RST] = "Level 3 reset";
		array[ELNRNG] = "Link number out of range";
		array[EUNATCH] = "Protocol driver not attached";
		array[ENOCSI] = "No CSI structure available";
		array[EL2HLT] = "Level 2 halted";
		array[EBADE] = "Invalid exchange";
		array[EBADR] = "Invalid request descriptor";
		array[EXFULL] = "Exchange full";
		array[ENOANO] = "No anode";
		array[EBADRQC] = "Invalid request code";
		array[EBADSLT] = "Invalid slot";
		array[EBFONT] = "Bad font file format";
		array[ENOSTR] = "Device not a stream";
		array[ENODATA] = "No data available";
		array[ETIME] = "Timer expired";
		array[ENOSR] = "Out of streams resources";
		array[ENONET] = "Machine is not on the network";
		array[ENOPKG] = "Package not installed";
		array[EREMOTE] = "Object is remote";
		array[ENOLINK] = "Link has been severed";
		array[EADV] = "Advertise error";
		array[ESRMNT] = "Srmount error";
		array[ECOMM] = "Communication error on send";
		array[EPROTO] = "Protocol error";
		array[EMULTIHOP] = "Multihop attempted";
		array[EDOTDOT] = "RFS specific error";
		array[EBADMSG] = "Not a data message";
		array[EOVERFLOW] = "Value too large to be stored in data type";
		array[ENOTUNIQ] = "Name not unique on network";
		array[EBADFD] = "File descriptor in bad state";
		array[EREMCHG] = "Remote address changed";
		array[ELIBACC] = "Can not access a needed shared library";
		array[ELIBBAD] = "Accessing a corrupted shared library";
		array[ELIBSCN] = ".lib section in a.out corrupted";
		array[ELIBMAX] = "Attempting to link in too many shared libraries";
		array[ELIBEXEC] = "Cannot exec a shared library directly";
		array[EILSEQ] = "Invalid or incomplete multibyte or wide character";
		array[ERESTART] = "Interrupted system call";
		array[ESTRPIPE] = "Streams pipe error";
		array[EUSERS] = "Too many users";
		array[ENOTSOCK] = "Socket operation on non-socket";
		array[EDESTADDRREQ] = "Destination address required";
		array[EMSGSIZE] = "Message too long";
		array[EPROTOTYPE] = "Protocol wrong type for socket";
		array[ENOPROTOOPT] = "Protocol not available";
		array[EPROTONOSUPPORT] = "Protocol not supported";
		array[ESOCKNOSUPPORT] = "Socket type not supported";
		array[EOPNOTSUPP] = "Operation not supported on transport endpoint";
		array[EPFNOSUPPORT] = "Protocol family not supported";
		array[EAFNOSUPPORT] = "Address family not supported";
		array[EADDRINUSE] = "Address already in use";
		array[EADDRNOTAVAIL] = "Cannot assign requested address";
		array[ENETDOWN] = "Network is down";
		array[ENETUNREACH] = "Network is unreachable";
		array[ENETRESET] = "Network dropped connection because of reset";
		array[ECONNABORTED] = "Software caused connection abort";
		array[ECONNRESET] = "Connection reset";
		array[ENOBUFS] = "No buffer space available";
		array[EISCONN] = "Transport endpoint is already connected";
		array[ENOTCONN] = "Transport endpoint is not connected";
		array[ESHUTDOWN] = "Cannot send after transport endpoint shutdown";
		array[ETOOMANYREFS] = "Too many references; cannot splice";
		array[ETIMEDOUT] = "Connection timed out";
		array[ECONNREFUSED] = "Connection refused";
		array[EHOSTDOWN] = "Host is down";
		array[EHOSTUNREACH] = "No route to host";
		array[EALREADY] = "Operation already in progress";
		array[EINPROGRESS] = "Operation now in progress";
		array[ESTALE] = "Stale file handle";
		array[EUCLEAN] = "Structure needs cleaning";
		array[ENOTMAM] = "Not a XENIX named type file";
		array[ENAVAIL] = "No XENIX semaphores available";
		array[EISNAM] = "Is a named type file";
		array[EREMOTEIO] = "Remote I/O error";
		array[EDQUOT] = "Quota exceeded";
		array[ENOMEDIUM] = "No medium found";
		array[EMEDIUMTYPE] = "Wrong medium type";
		array[ECANCELED] = "Operation canceled";
		array[ENOKEY] = "Required key not available";
		array[EKEYEXPIRED] = "Key has expired";
		array[EKEYREVOKED] = "Key has been revoked";
		array[EKEYREJECTED] = "Key was rejected by service";
		array[EOWNERDEAD] = "Owner died";
		array[ENOTRECOVERABLE] = "State not recoverable";
		array[ERFKILL] = "Operation not possible due to RF-kill";
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

ALIAS(strdup, __strdup);
