#include "rtld.hpp"
#include "object_storage.hpp"
#include "shared_object.hpp"
#include "dlfcn.h"
#include "tcb.hpp"

namespace {
	SharedObject* object_from_addr(uintptr_t addr) {
		auto guard = OBJECT_STORAGE.lock();
		for (auto object : (*guard)->objects) {
			for (auto& phdr : object->phdrs_vec) {
				if (phdr.p_type != PT_LOAD) {
					continue;
				}
				if (addr >= object->base + phdr.p_vaddr &&
					addr < object->base + phdr.p_vaddr + phdr.p_memsz) {
					return object;
				}
			}
		}

		return nullptr;
	}

	hz::spinlock<const char*> LAST_ERROR {nullptr};

	const char* OBJECT_NOT_FOUND = "shared object not found";
	const char* SYMBOL_NOT_FOUND = "symbol not found";

	void set_error(const char* error) {
		*LAST_ERROR.lock() = error;
	}

	void set_error(LoadError error) {
		switch (error) {
			case LoadError::Success:
				__builtin_unreachable();
			case LoadError::NotFound:
				set_error("file not found");
				break;
			case LoadError::InvalidElf:
				set_error("file is not a valid elf object");
				break;
			case LoadError::InvalidArch:
				set_error("file has different architecture");
				break;
			case LoadError::NoMemory:
				set_error("out of memory");
				break;
			default:
				set_error("unknown error");
				break;
		}
	}
}

void* __dlapi_open(const char* filename, int flags, uintptr_t return_addr) {
	if (!filename) {
		return (*OBJECT_STORAGE.lock())->objects[0];
	}

	hz::string_view name {filename};
	auto* origin = object_from_addr(return_addr);
	__ensure(origin);

	auto guard = OBJECT_STORAGE.lock();

	if (name.contains('/')) {
		for (auto object : origin->local_scope) {
			if (object->path == name) {
				return object;
			}
		}

		for (auto object : (*guard)->global_scope) {
			if (object->path == name) {
				return object;
			}
		}

		// todo promote to global scope with RTLD_NOLOAD | RTLD_GLOBAL

		if (flags & RTLD_NOLOAD) {
			set_error(OBJECT_NOT_FOUND);
			return nullptr;
		}

		hz::string<Allocator> path {Allocator {}};
		path = name;
		auto result = (*guard)->load_object_at_path(origin, path);
		if (!result) {
			set_error(result.error());
			return nullptr;
		}

		auto* object = result.value();
		auto status = (*guard)->load_dependencies(object, flags & RTLD_GLOBAL);
		if (status != LoadError::Success) {
			// object is destroyed by load_dependencies
			set_error(status);
			return nullptr;
		}

		if (!(flags & RTLD_GLOBAL)) {
			origin->local_scope.push_back(object);
		}

		return object;
	}
	else {
		for (auto object : origin->local_scope) {
			if (object->name == name) {
				return object;
			}
			else if (object->path.as_view().ends_with(name)) {
				return object;
			}
		}

		for (auto object : (*guard)->global_scope) {
			if (object->name == name) {
				return object;
			}
			else if (object->path.as_view().ends_with(name)) {
				return object;
			}
		}

		// todo promote to global scope with RTLD_NOLOAD | RTLD_GLOBAL

		if (flags & RTLD_NOLOAD) {
			set_error(OBJECT_NOT_FOUND);
			return nullptr;
		}

		auto result = (*guard)->load_object_with_name(origin, name);
		if (!result) {
			set_error(result.error());
			return nullptr;
		}

		auto* object = result.value();
		auto status = (*guard)->load_dependencies(object, flags & RTLD_GLOBAL);
		if (status != LoadError::Success) {
			// object is destroyed by load_dependencies
			set_error(status);
			return nullptr;
		}

		if (!(flags & RTLD_GLOBAL)) {
			origin->local_scope.push_back(object);
		}

		return object;
	}
}

int __dlapi_close([[maybe_unused]] void* handle) {
	return 0;
}

void* __dlapi_get_sym(void* __restrict handle, const char* __restrict symbol) {
	if (handle == RTLD_DEFAULT) {
		auto guard = OBJECT_STORAGE.lock();
		auto sym = (*guard)->lookup(nullptr, symbol, LookupPolicy::None);
		if (!sym) {
			set_error(SYMBOL_NOT_FOUND);
			return nullptr;
		}
		return reinterpret_cast<void*>(sym->object->base + sym->sym->st_value);
	}
	else if (handle == RTLD_NEXT) {
		auto* origin = object_from_addr(
			reinterpret_cast<uintptr_t>(
				__builtin_extract_return_addr(
					__builtin_return_address(0))));
		__ensure(origin);
		auto guard = OBJECT_STORAGE.lock();
		auto sym = (*guard)->lookup(origin, symbol, LookupPolicy::IgnoreLocal);
		if (!sym) {
			set_error(SYMBOL_NOT_FOUND);
			return nullptr;
		}
		return reinterpret_cast<void*>(sym->object->base + sym->sym->st_value);
	}

	auto* object = static_cast<SharedObject*>(handle);
	auto guard = OBJECT_STORAGE.lock();
	auto sym = (*guard)->lookup(object, symbol, LookupPolicy::LocalAndDeps);
	if (!sym) {
		set_error(SYMBOL_NOT_FOUND);
		return nullptr;
	}
	return reinterpret_cast<void*>(sym->object->base + sym->sym->st_value);
}

char* __dlapi_get_error() {
	auto guard = LAST_ERROR.lock();
	auto error = *guard;
	*guard = nullptr;
	return const_cast<char*>(error);
}

void __dlapi_exit() {
	auto guard = OBJECT_STORAGE.lock();
	(*guard)->destruct_objects();
	guard->destroy();
}

bool __dlapi_create_tcb(void** tcb, void** tp) {
	auto guard = OBJECT_STORAGE.lock();
	uintptr_t tcb_offset = ((*guard)->total_initial_tls_size + alignof(Tcb) - 1) & ~(alignof(Tcb) - 1);
	uintptr_t total_tls_size = tcb_offset + sizeof(Tcb);

	auto* mem = Allocator::allocate(total_tls_size);
	if (!mem) {
		return false;
	}
	auto* tcb_ptr = new (reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mem) + tcb_offset)) Tcb {};
	*tcb = tcb_ptr;
	(*guard)->init_tls(tcb_ptr);

#if defined(__x86_64__) || defined(__i386__)
	*tp = tcb_ptr;
#else
#error missing architecture specific code
#endif
	return true;
}

void __dlapi_destroy_tcb(void* tcb) {
	auto guard = OBJECT_STORAGE.lock();
	uintptr_t tcb_offset = ((*guard)->total_initial_tls_size + alignof(Tcb) - 1) & ~(alignof(Tcb) - 1);
	Allocator::deallocate(static_cast<char*>(tcb) - tcb_offset);
}
