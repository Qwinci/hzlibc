#include "string.h"
#include "utils.hpp"
#include "signal.h"
#include "string.hpp"

EXPORT size_t strnlen(const char* str, size_t max_len) {
	size_t len = 0;
	for (; max_len && *str; ++str, --max_len) ++len;
	return len;
}

EXPORT char* stpcpy(char* __restrict dest, const char* src) {
	for (; *src;) {
		*dest++ = *src++;
	}
	*dest = 0;
	return dest;
}

EXPORT char* strtok_r(char* __restrict str, const char* __restrict delim, char** __restrict save_ptr) {
	return internal::strtok_r(str, delim, save_ptr);
}

EXPORT int strcoll_l(const char* lhs, const char* rhs, locale_t locale) {
	//println("strcoll_l ignores locale");
	return strcoll(lhs, rhs);
}

EXPORT size_t strxfrm_l(char* __restrict dest, const char* __restrict src, size_t count, locale_t locale) {
	//println("strxfrm_l ignores locale");
	return strxfrm(dest, src, count);
}

EXPORT char* strerror_l(int err_num, locale_t locale) {
	//println("strerror_l ignores locale");
	return strerror(err_num);
}

EXPORT char* strsignal(int sig) {
	const char* str;
#define CASE(name) case name: str = #name; break
	switch (sig) {
		CASE(SIGHUP);
		CASE(SIGINT);
		CASE(SIGQUIT);
		CASE(SIGILL);
		CASE(SIGTRAP);
		CASE(SIGABRT);
		CASE(SIGBUS);
		CASE(SIGFPE);
		CASE(SIGKILL);
		CASE(SIGUSR1);
		CASE(SIGSEGV);
		CASE(SIGUSR2);
		CASE(SIGPIPE);
		CASE(SIGALRM);
		CASE(SIGTERM);
		CASE(SIGSTKFLT);
		CASE(SIGCHLD);
		CASE(SIGCONT);
		CASE(SIGSTOP);
		CASE(SIGTSTP);
		CASE(SIGTTIN);
		CASE(SIGTTOU);
		CASE(SIGURG);
		CASE(SIGXCPU);
		CASE(SIGXFSZ);
		CASE(SIGVTALRM);
		CASE(SIGPROF);
		CASE(SIGWINCH);
		CASE(SIGIO);
		CASE(SIGPWR);
		CASE(SIGSYS);
		default:
			println("Unknown signal number ", sig);
			str = "Unknown signal number";
			break;
	}
#undef CASE
	return const_cast<char*>(str);
}

ALIAS(strtok_r, __strtok_r);
ALIAS(strcoll_l, __strcoll_l);
ALIAS(strxfrm_l, __strxfrm_l);
