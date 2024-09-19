#include "sys/ptrace.h"
#include "utils.hpp"
#include "sys.hpp"
#include "errno.h"

EXPORT long ptrace(int op, ...) {
	va_list ap;
	va_start(ap, op);
	auto pid = va_arg(ap, pid_t);
	auto addr = va_arg(ap, void*);
	auto data = va_arg(ap, void*);
	va_end(ap);

	long ret;
	if (op >= PTRACE_PEEKTEXT && op <= PTRACE_PEEKUSER) {
		data = &ret;
	}

	long out;
	if (auto err = sys_ptrace(static_cast<__ptrace_request>(op), pid, addr, data, &out)) {
		errno = err;
		return -1;
	}
	else if (op >= PTRACE_PEEKTEXT && op <= PTRACE_PEEKUSER) {
		errno = 0;
		return ret;
	}
	else {
		return out;
	}
}
