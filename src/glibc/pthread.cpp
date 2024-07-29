#include "pthread.h"
#include "utils.hpp"
#include "sys.hpp"
#include "internal/tcb.hpp"

EXPORT int pthread_setname_np(pthread_t thread, const char* name) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	return sys_thread_set_name(tcb->tid, name);
}

EXPORT int pthread_getname_np(pthread_t thread, char* name, size_t len) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	return sys_thread_get_name(tcb->tid, name, len);
}

EXPORT int pthread_setaffinity_np(pthread_t thread, size_t cpu_set_size, const cpu_set_t* cpu_set) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	return sys_sched_setaffinity(tcb->tid, cpu_set_size, cpu_set);
}

EXPORT int pthread_getaffinity_np(pthread_t thread, size_t cpu_set_size, cpu_set_t* cpu_set) {
	auto* tcb = reinterpret_cast<Tcb*>(thread);
	return sys_sched_getaffinity(tcb->tid, cpu_set_size, cpu_set);
}
