#pragma once
#include <stdint.h>
#include <hz/string_view.hpp>
#include <hz/spinlock.hpp>

enum class Fmt {
	Dec,
	Hex
};

struct Log {
	Log& operator<<(hz::string_view str);

	Log& operator<<(uintptr_t value);

	constexpr Log& operator<<(Fmt fmt) {
		_fmt = fmt;
		return *this;
	}

private:
	Fmt _fmt;
};

extern hz::spinlock<Log> LOG;

template<typename... Args>
void print(Args&&... args) {
	auto guard = LOG.lock();
	((*guard << args), ...);
}

template<typename... Args>
void println(Args&&... args) {
	auto guard = LOG.lock();
	((*guard << args), ...);
	*guard << "\n";
}

template<typename... Args>
[[noreturn]] void panic(Args&&... args) {
	auto guard = LOG.lock();
	*guard << "libc panic: ";
	((*guard << args), ...);
	*guard << "\n";
	__builtin_trap();
}
