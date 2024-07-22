#include "thread.hpp"
#include "tcb.hpp"
#include "sys.hpp"

extern "C" [[noreturn]] void hzlibc_thread_entry(void* (*fn)(void* arg), void* arg) {
	auto* tcb = get_current_tcb();

	while (!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED)) {
		sys_futex_wait(&tcb->tid, 0, nullptr);
	}

	auto ret = fn(arg);
	tcb->exit_status = ret;
	tcb->exited.store(1, hz::memory_order::release);
	sys_futex_wake(tcb->exited.data());

	sys_exit_thread();
}
