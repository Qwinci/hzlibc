#include "bsd/stdlib.h"
#include "utils.hpp"
#include "mutex.hpp"
#include "sys.hpp"
#include "errno.h"
#include "cha_cha.hpp"
#include "sys/sysinfo.h"
#include <hz/algorithm.hpp>

#define memcpy __builtin_memcpy

namespace {
	Nonce NONCE {};
	uint32_t KEY[8] {};
	bool INITIALIZED {};
	Mutex LOCK {};
}

EXPORT uint32_t arc4random() {
	uint32_t value;
	arc4random_buf(&value, 4);
	return value;
}

EXPORT void arc4random_buf(void* buf, size_t num_bytes) {
	auto guard = LOCK.lock();
	if (!INITIALIZED) {
		ssize_t ret;
		while (auto err = sys_getrandom(KEY, 32, 0, &ret)) {
			if (err == EINTR) {
				continue;
			}
			panic("arc4random_buf: sys_getrandom failed");
		}
	}

	uint32_t counter = 0;
	for (size_t i = 0; i < num_bytes; i += 64) {
		auto state = cha_cha20_block(KEY, counter++, NONCE.inner);
		size_t to_copy = hz::min(num_bytes - i, size_t {64});
		memcpy(static_cast<char*>(buf) + i, state.state, to_copy);
	}
	NONCE.use();
}

EXPORT int getloadavg(double load_avg[], int num_elem) {
	if (num_elem <= 0) {
		return num_elem ? -1 : 0;
	}

	struct sysinfo info {};
	if (auto err = sys_sysinfo(&info)) {
		errno = err;
		return -1;
	}

	num_elem = hz::min(num_elem, 3);
	for (int i = 0; i < num_elem; ++i) {
		load_avg[i] = static_cast<double>(info.loads[i]) / (1 << SI_LOAD_SHIFT);
	}
	return num_elem;
}
