#include "rtld.hpp"
#include "object_storage.hpp"
#include "shared_object.hpp"
#include "dlfcn.h"
#include "tcb.hpp"

namespace {
	SharedObject* object_from_addr(uintptr_t addr) {
		auto guard = OBJECT_STORAGE->objects.lock();
		for (auto object : *guard) {
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

void set_debug_state_to_load();
void set_debug_state_to_normal();

extern "C" int __cxa_atexit(void (*func)(void*), void* arg, void* dso_handle);

void __dlapi_exit(void*);

void* __dlapi_open(const char* filename, int flags, uintptr_t return_addr) {
	if (!filename) {
		return OBJECT_STORAGE->objects.get_unsafe()[0];
	}

	hz::string_view name {filename};
	auto* origin = object_from_addr(return_addr);
	__ensure(origin);

	if (name == "ld-linux.so.2" || name == "ld-linux-x86-64.so.2" || name == "libc.so.6" ||
		name == "libpthread.so.0") {
		name = "libc.so";
	}

	hz::vector<SharedObject*, Allocator> init_list {Allocator {}};
	SharedObject* new_object;
	{
		auto storage_guard = OBJECT_STORAGE->lock.lock();

		if (name.contains('/')) {
			auto guard = OBJECT_STORAGE->objects.lock();

			for (auto object : *guard) {
				if (object->path == name) {
					return object;
				}
			}

			// todo promote to global scope with RTLD_NOLOAD | RTLD_GLOBAL

			if (flags & RTLD_NOLOAD) {
				set_error(OBJECT_NOT_FOUND);
				return nullptr;
			}

			set_debug_state_to_load();

			hz::string<Allocator> path {Allocator {}};
			path = name;
			auto result = OBJECT_STORAGE->load_object_at_path(origin, path);
			if (!result) {
				set_error(result.error());
				set_debug_state_to_normal();
				return nullptr;
			}
			OBJECT_STORAGE->add_object(result.value());

			new_object = result.value();
			auto status = OBJECT_STORAGE->load_dependencies(new_object, flags & RTLD_GLOBAL, false);
			if (status != LoadError::Success) {
				// object is destroyed by load_dependencies
				guard->pop_back();
				set_error(status);
				set_debug_state_to_normal();
				return nullptr;
			}
		}
		else {
			auto guard = OBJECT_STORAGE->objects.lock();

			for (auto object : *guard) {
				if (object->name == name || object->path.as_view().ends_with(name)) {
					return object;
				}
			}

			// todo promote to global scope with RTLD_NOLOAD | RTLD_GLOBAL

			if (flags & RTLD_NOLOAD) {
				set_error(OBJECT_NOT_FOUND);
				return nullptr;
			}

			set_debug_state_to_load();

			auto result = OBJECT_STORAGE->load_object_with_name(origin, name);
			if (!result) {
				set_error(result.error());
				set_debug_state_to_normal();
				return nullptr;
			}
			OBJECT_STORAGE->add_object(result.value());

			new_object = result.value();
			auto status = OBJECT_STORAGE->load_dependencies(new_object, flags & RTLD_GLOBAL, false);
			if (status != LoadError::Success) {
				// object is destroyed by load_dependencies
				guard->pop_back();
				set_error(status);
				set_debug_state_to_normal();
				return nullptr;
			}
		}

		if (!(flags & RTLD_GLOBAL)) {
			origin->local_scope.push_back(new_object);
		}

		set_debug_state_to_normal();
		OBJECT_STORAGE->init_tls(get_current_tcb(), true);

		// todo this is a hack to avoid deadlocks with constructors calling dlopen
		init_list = std::move(OBJECT_STORAGE->init_list);
	}

	for (size_t i = init_list.size(); i > 0; --i) {
		if (!init_list[i - 1]->initialized) {
			if (init_list[i - 1]->executable) {
				__cxa_atexit(__dlapi_exit, nullptr, nullptr);
			}

			{
				auto guard = OBJECT_STORAGE->objects.lock();
				OBJECT_STORAGE->destruct_list.push_back(init_list[i - 1]);
			}

			init_list[i - 1]->run_init();
			init_list[i - 1]->initialized = true;
		}
	}

	return new_object;
}

int __dlapi_close(void* handle) {
	return 0;
}

void* __dlapi_get_sym(void* __restrict handle, const char* __restrict symbol) {
	auto guard = OBJECT_STORAGE->objects.lock();

	if (handle == RTLD_DEFAULT) {
		auto sym = OBJECT_STORAGE->lookup(nullptr, symbol, LookupPolicy::None);
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
		auto sym = OBJECT_STORAGE->lookup(origin, symbol, LookupPolicy::IgnoreLocal);
		if (!sym) {
			set_error(SYMBOL_NOT_FOUND);
			return nullptr;
		}
		return reinterpret_cast<void*>(sym->object->base + sym->sym->st_value);
	}

	auto* object = static_cast<SharedObject*>(handle);
	auto sym = OBJECT_STORAGE->lookup(object, symbol, LookupPolicy::LocalAndDeps);
	if (!sym) {
		set_error(SYMBOL_NOT_FOUND);
		return nullptr;
	}
	return reinterpret_cast<void*>(sym->object->base + sym->sym->st_value);
}

int __dlapi_dladdr(uintptr_t addr, DlInfo* info) {
	auto object = object_from_addr(addr);
	if (!object) {
		return 0;
	}
	info->object_path = object->path.data();
	info->object_base = object->base;

	for (uintptr_t i = 1; i < object->num_symbols; ++i) {
		auto& sym = object->symtab[i];
		if (sym.st_shndx == SHN_UNDEF ||
			ELF_ST_BIND(sym.st_info) == STB_LOCAL) {
			continue;
		}

		auto start = object->base + sym.st_value;
		if (addr < start) {
			continue;
		}

		if (sym.st_size) {
			auto end = start + sym.st_size;
			if (addr >= end) {
				continue;
			}
		}
		else {
			if (addr != start) {
				continue;
			}
		}

		info->symbol_name = object->strtab + sym.st_name;
		info->symbol_address = object->base + sym.st_value;
		info->symbol = &sym;
		info->map = reinterpret_cast<link_map*>(&object->link_map);
		return 1;
	}

	info->symbol_name = nullptr;
	info->symbol_address = 0;
	info->map = reinterpret_cast<link_map*>(&object->link_map);

	return 1;
}

int __dlapi_dlinfo(void* __restrict handle, int request, void* __restrict info) {
	__ensure(!"__dlapi_dlinfo is not implemented");
}

EXPORT int _dl_find_object(void* addr, struct dl_find_object* result) {
	auto guard = OBJECT_STORAGE->objects.lock();
	for (auto object : *guard) {
		if (object->base > reinterpret_cast<uintptr_t>(addr) ||
			object->end < reinterpret_cast<uintptr_t>(addr)) {
			continue;
		}

		result->dlfo_flags = 0;
		result->dlfo_map_start = reinterpret_cast<void*>(object->base);
		result->dlfo_map_end = reinterpret_cast<void*>(object->end);
		result->dlfo_link_map = reinterpret_cast<link_map*>(&object->link_map);

		for (auto& phdr : object->phdrs_vec) {
			if (phdr.p_type == DLFO_EH_SEGMENT_TYPE) {
				result->dlfo_eh_frame = reinterpret_cast<void*>(object->base + phdr.p_vaddr);
				break;
			}
		}

#if DLFO_STRUCT_HAS_EH_DBASE
		result->dlfo_eh_dbase = nullptr;
#endif
#if DLFO_STRUCT_HAS_EH_COUNT
		result->dlfo_eh_count = 0;
#endif
		return 0;
	}
	return -1;
}

void* rtld_memset(void* __restrict dest, int ch, size_t size);
void* rtld_memcpy(void* __restrict dest, const void* __restrict src, size_t size);

void* access_tls_for_object(SharedObject* object) {
	auto* tcb = get_current_tcb();

	if (object->tls_module_index >= tcb->dtv.size()) {
		auto guard = OBJECT_STORAGE->runtime_tls_map.lock();
		tcb->dtv.resize(guard->size());
	}

	auto& entry = tcb->dtv[object->tls_module_index];
	if (!entry) {
		auto* mem = static_cast<char*>(Allocator::allocate(object->tls_size));
		__ensure(mem);
		rtld_memset(
			mem + object->tls_image_size,
			0,
			object->tls_size - object->tls_image_size);
		rtld_memcpy(mem, object->tls_image, object->tls_image_size);
		entry = mem;
	}

	return entry;
}

void* try_access_tls_for_object(SharedObject* object) {
	auto* tcb = get_current_tcb();
	if (object->tls_module_index >= tcb->dtv.size()) {
		return nullptr;
	}
	return tcb->dtv[object->tls_module_index];
}

struct TlsEntry {
	SharedObject* object;
	uintptr_t offset;
};

#ifdef __i386__

extern "C" [[gnu::regparm(1)]] EXPORT void* ___tls_get_addr(TlsEntry* entry) {
	return static_cast<char*>(access_tls_for_object(entry->object)) + entry->offset;
}

#else

extern "C" EXPORT void* __tls_get_addr(TlsEntry* entry) {
	return static_cast<char*>(access_tls_for_object(entry->object)) + entry->offset;
}

#endif

int __dlapi_iterate_phdr(
	int (*callback)(
		dl_phdr_info* info,
		size_t size,
		void* data),
	void* data) {
	auto guard = OBJECT_STORAGE->objects.lock();
	int ret = 0;
	for (size_t i = 0; i < guard->size(); ++i) {
		auto object = (*guard)[i];
		dl_phdr_info info {
			.dlpi_addr = object->base,
			.dlpi_name = object->name.data(),
			.dlpi_phdr = object->phdrs_vec.data(),
			.dlpi_phnum = static_cast<ElfW(Half)>(object->phdrs_vec.size()),
			.dlpi_adds = guard->size(),
			.dlpi_subs = 0,
			.dlpi_tls_modid = object->tls_module_index,
			.dlpi_tls_data = try_access_tls_for_object(object)
		};
		ret = callback(&info, sizeof(dl_phdr_info), data);
		if (ret) {
			return ret;
		}
	}
	return ret;
}

char* __dlapi_get_error() {
	auto guard = LAST_ERROR.lock();
	auto error = *guard;
	*guard = nullptr;
	return const_cast<char*>(error);
}

void __dlapi_exit(void*) {
	auto guard = OBJECT_STORAGE->lock.lock();
	OBJECT_STORAGE->destruct_objects();
}

void __dlapi_destroy() {
	OBJECT_STORAGE->unload_objects();
	OBJECT_STORAGE.destroy();
}

bool __dlapi_create_tcb(void** tcb, void** tp) {
	auto guard = OBJECT_STORAGE->lock.lock();
	uintptr_t tcb_offset = (OBJECT_STORAGE->total_initial_tls_size + alignof(Tcb) - 1) & ~(alignof(Tcb) - 1);
	uintptr_t total_tls_size = tcb_offset + sizeof(Tcb);

	auto* mem = Allocator::allocate(total_tls_size);
	if (!mem) {
		return false;
	}
	Tcb* tcb_ptr;
	if constexpr (TLS_ABOVE_TP) {
		tcb_ptr = new (mem) Tcb {};
	}
	else {
		tcb_ptr = new (reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(mem) + tcb_offset)) Tcb {};
	}
	*tcb = tcb_ptr;
	OBJECT_STORAGE->init_tls(tcb_ptr, false);

#if defined(__x86_64__) || defined(__i386__)
	*tp = tcb_ptr;
#elif defined(__aarch64__)
	*tp = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(tcb_ptr) + sizeof(Tcb) - 0x10);
#else
#error missing architecture specific code
#endif
	return true;
}

void __dlapi_destroy_tcb(void* tcb) {
	auto guard = OBJECT_STORAGE->lock.lock();
	uintptr_t tcb_offset = (OBJECT_STORAGE->total_initial_tls_size + alignof(Tcb) - 1) & ~(alignof(Tcb) - 1);
	Allocator::deallocate(static_cast<char*>(tcb) - tcb_offset);
}
