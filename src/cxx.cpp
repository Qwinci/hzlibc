#include "log.hpp"
#include "allocator.hpp"

void* __dso_handle = nullptr;

extern "C" [[gnu::used]] void __cxa_pure_virtual() {
	panic("__cxa_pure_virtual called");
}

asm(".hidden _ZdlPv");

#if UINTPTR_MAX == UINT64_MAX
asm(".hidden _Znwm");
asm(".hidden _ZdlPvm");
#else
asm(".hidden _Znwj");
asm(".hidden _ZdlPvj");
#endif

void* operator new(size_t size) {
	return Allocator::allocate(size);
}

void operator delete(void* ptr) {
	Allocator::deallocate(ptr);
}

void operator delete(void* ptr, size_t) {
	Allocator::deallocate(ptr);
}
