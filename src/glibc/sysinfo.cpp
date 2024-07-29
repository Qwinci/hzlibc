#include "sys/sysinfo.h"
#include "utils.hpp"
#include "sys.hpp"

EXPORT int get_nprocs() {
	cpu_set_t set {};
	if (sys_sched_getaffinity(0, sizeof(set), &set)) {
		println("get_nprocs: sys_sched_getaffinity failed");
		return 1;
	}
	int count = 0;
	for (int i = 0; i < CPU_SETSIZE; ++i) {
		if (set.__bits[__CPUELT(i)] & __CPUMASK(i)) {
			++count;
		}
	}
	return count;
}
