#include "log.hpp"
#include "sys.hpp"

Log& Log::operator<<(hz::string_view str) {
	sys_libc_log(str);
	return *this;
}

Log& Log::operator<<(uintptr_t value) {
	char buf[20];
	char* ptr = buf + 20;

	int base = _fmt == Fmt::Dec ? 10 : 16;

	do {
		*--ptr = "0123456789ABCDEF"[value % base];
		value /= base;
	} while (value);

	return operator<<(hz::string_view {ptr, static_cast<size_t>((buf + 20) - ptr)});
}

hz::spinlock<Log> LOG {};
