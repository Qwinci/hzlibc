#include "utils.hpp"
#include "allocator.hpp"
#include <hz/vector.hpp>

using DestructorFn = void (*)(void*);

namespace {
	struct Destructor {
		DestructorFn fn;
		void* object;
	};

	hz::vector<Destructor, Allocator> DESTRUCTOR_LIST {Allocator {}};
}

extern "C" EXPORT int __cxa_thread_atexit_impl(DestructorFn fn, void* object, void* dso_handle) {
	DESTRUCTOR_LIST.push_back({
		.fn = fn,
		.object = object
	});
	return 0;
}

void call_cxx_tls_destructors() {
	for (size_t i = DESTRUCTOR_LIST.size(); i > 0; --i) {
		auto& destructor = DESTRUCTOR_LIST[i];
		destructor.fn(destructor.object);
	}
	DESTRUCTOR_LIST.clear();
}
