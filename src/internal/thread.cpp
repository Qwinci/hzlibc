#include "thread.hpp"
#include "tcb.hpp"
#include "sys.hpp"
#include "rtld/rtld.hpp"

void pthread_call_destructors();
void call_cxx_tls_destructors();

[[noreturn]] void hzlibc_thread_exit(void* ret) {
	auto* tcb = get_current_tcb();

	call_cxx_tls_destructors();
	pthread_call_destructors();

	if (tcb->detached) {
		__dlapi_destroy_tcb(tcb);
	}

	tcb->exit_status = ret;
	tcb->exited.store(1, hz::memory_order::release);
	sys_futex_wake_all(tcb->exited.data());

	sys_exit_thread();
}

extern "C" [[noreturn]] void hzlibc_thread_entry(void* (*fn)(void* arg), void* arg) {
	auto* tcb = get_current_tcb();

	while (!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED)) {
		sys_futex_wait(&tcb->tid, 0, nullptr);
	}

	auto ret = fn(arg);
	hzlibc_thread_exit(ret);
}
