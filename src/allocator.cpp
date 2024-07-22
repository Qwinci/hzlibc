#include "allocator.hpp"

#include "utils.hpp"
#include "sys.hpp"
#include "sys/mman.h"
#include <hz/slab.hpp>

#define memcpy __builtin_memcpy

namespace {
	struct ArenaAllocator {
		static void* allocate(size_t size) {
			size = (size + 0x1000 - 1) & ~(0x1000 - 1);
			void* ret;
			if (sys_mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0, &ret)) {
				return nullptr;
			}
			return ret;
		}

		static void deallocate(void* ptr, size_t size) {
			size = (size + 0x1000 - 1) & ~(0x1000 - 1);
			__ensure(sys_munmap(ptr, size) == 0);
		}
	};

	constinit hz::slab_allocator<ArenaAllocator> ALLOCATOR {ArenaAllocator {}};
}

void* allocate(size_t size) {
	return ALLOCATOR.alloc(size);
}

void* reallocate(void* old, size_t new_size) {
	if (!new_size) {
		deallocate(old);
		return nullptr;
	}

	void* ptr = allocate(new_size);
	if (!ptr) {
		return nullptr;
	}
	if (old) {
		auto old_size = ALLOCATOR.get_size_for_allocation(old);
		memcpy(ptr, old, new_size < old_size ? new_size : old_size);
		deallocate(old);
	}
	return ptr;
}

void deallocate(void* ptr) {
	ALLOCATOR.free(ptr);
}
