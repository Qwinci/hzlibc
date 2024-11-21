#include "utils.hpp"
#include "allocator.hpp"
#include <hz/vector.hpp>
#include <hz/manually_init.hpp>

using DestructorFn = void (*)(void*);

namespace {
	struct Destructor {
		DestructorFn fn;
		void* object;
	};

	thread_local hz::manually_init<hz::vector<Destructor, Allocator>> DESTRUCTOR_LIST {};
	thread_local bool initialized = false;
}

extern "C" EXPORT int __cxa_thread_atexit_impl(DestructorFn fn, void* object, void* dso_handle) {
	if (!initialized) {
		DESTRUCTOR_LIST.initialize(Allocator {});
		initialized = true;
	}

	DESTRUCTOR_LIST->push_back({
		.fn = fn,
		.object = object
	});
	return 0;
}

void call_cxx_tls_destructors() {
	if (!initialized) {
		return;
	}

	auto list = DESTRUCTOR_LIST.data();
	for (size_t i = DESTRUCTOR_LIST->size(); i > 0; --i) {
		auto& destructor = (*DESTRUCTOR_LIST)[i - 1];
		destructor.fn(destructor.object);
	}
	DESTRUCTOR_LIST.destroy();
}
