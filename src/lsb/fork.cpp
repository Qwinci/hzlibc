#include "utils.hpp"
#include "allocator.hpp"
#include "mutex.hpp"
#include <hz/vector.hpp>

namespace {
	struct AtForkFn {
		void (*prepare)();
		void (*parent)();
		void (*child)();
	};

	hz::vector<AtForkFn, Allocator> AT_FORK {Allocator {}};
	Mutex LOCK {};
}

void call_atfork_prepare() {
	auto guard = LOCK.lock();
	for (size_t i = AT_FORK.size(); i > 0; --i) {
		auto& fn = AT_FORK[i - 1];
		if (fn.prepare) {
			fn.prepare();
		}
	}
}

void call_atfork_parent() {
	auto guard = LOCK.lock();
	for (auto& fn : AT_FORK) {
		if (fn.parent) {
			fn.parent();
		}
	}
}

void call_atfork_child() {
	auto guard = LOCK.lock();
	for (auto& fn : AT_FORK) {
		if (fn.child) {
			fn.child();
		}
	}
}

extern "C" EXPORT int __register_atfork(
	void (*prepare)(),
	void (*parent)(),
	void (*child)(),
	void* dso_handle) {
	auto guard = LOCK.lock();
	AT_FORK.push_back({
		.prepare = prepare,
		.parent = parent,
		.child = child
	});
	return 0;
}
