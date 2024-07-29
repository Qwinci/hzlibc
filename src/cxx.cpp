#include "log.hpp"
#include "allocator.hpp"

void* __dso_handle = nullptr;

extern "C" [[gnu::used]] void __cxa_pure_virtual() {
	panic("__cxa_pure_virtual called");
}

asm(".hidden _ZdlPv");
asm(".hidden _ZdaPv");

#if UINTPTR_MAX == UINT64_MAX
asm(".hidden _Znwm");
asm(".hidden _Znam");
asm(".hidden _ZdlPvm");
asm(".hidden _ZdaPvm");
#else
asm(".hidden _Znwj");
asm(".hidden _Znaj");
asm(".hidden _ZdlPvj");
asm(".hidden _ZdaPvj");
#endif

void* operator new(size_t size) {
	return Allocator::allocate(size);
}

void* operator new[](size_t size) {
	return Allocator::allocate(size);
}

void operator delete(void* ptr) {
	Allocator::deallocate(ptr);
}

void operator delete[](void* ptr) {
	Allocator::deallocate(ptr);
}

void operator delete(void* ptr, size_t) {
	Allocator::deallocate(ptr);
}

void operator delete[](void* ptr, size_t) {
	Allocator::deallocate(ptr);
}
