#include "object_storage.hpp"
#include "shared_object.hpp"
#include "sys.hpp"
#include "fcntl.h"
#include "stdio.h"
#include "sys/mman.h"

#define memset __builtin_memset

// todo unhardcode this
namespace {
	constinit hz::string_view SYSTEM_LIBRARY_PATHS[4] {};

	void init_system_paths() {
		SYSTEM_LIBRARY_PATHS[0] =
#if UINTPTR_MAX == UINT64_MAX
		"/usr/local/lib64";
#else
		"/usr/local/lib32";
#endif
		SYSTEM_LIBRARY_PATHS[1] = "/usr/local/lib";
		SYSTEM_LIBRARY_PATHS[2] =
#if UINTPTR_MAX == UINT64_MAX
		"/usr/lib64";
#else
		"/usr/lib32";
#endif
		SYSTEM_LIBRARY_PATHS[3] = "/usr/lib";
	}
}

ObjectStorage::ObjectStorage() {
	init_system_paths();
}

void ObjectStorage::add_object(SharedObject* object) {
	objects.push_back(object);
}

namespace {
	bool try_read(int fd, void* buf, size_t count) {
		ssize_t read_count;
		auto res = sys_read(fd, buf, count, &read_count);
		return res == 0 && static_cast<size_t>(read_count) == count;
	}

	bool try_seek(int fd, off64_t offset, int whence) {
		off64_t pos;
		return sys_lseek(fd, offset, whence, &pos) == 0;
	}

	struct UniqueFd {
		constexpr UniqueFd(const UniqueFd&) = delete;
		constexpr UniqueFd& operator=(const UniqueFd&) = delete;

		constexpr explicit UniqueFd(int fd) : fd {fd} {}

		constexpr UniqueFd(UniqueFd&& other) : fd {other.fd} {
			other.fd = -1;
		}

		inline UniqueFd& operator=(UniqueFd&& other) {
			if (fd != -1) {
				__ensure(sys_close(fd) == 0);
			}
			fd = other.fd;
			other.fd = -1;
			return *this;
		}

		inline ~UniqueFd() {
			if (fd != -1) {
				__ensure(sys_close(fd) == 0);
			}
		}

		constexpr operator int() const { // NOLINT(*-explicit-constructor)
			return fd;
		}

	private:
		int fd;
	};
}

static void* rtld_memset(void* __restrict dest, int ch, size_t size) {
	auto* ptr = static_cast<unsigned char*>(dest);
	for (; size; --size) {
		*ptr++ = static_cast<unsigned char>(ch);
	}
	return dest;
}

static void* rtld_memcpy(void* __restrict dest, const void* __restrict src, size_t size) {
	auto* dest_ptr = static_cast<unsigned char*>(dest);
	auto* src_ptr = static_cast<const unsigned char*>(src);
	for (; size; --size) {
		*dest_ptr++ = *src_ptr++;
	}
	return dest;
}

hz::result<SharedObject*, LoadError> ObjectStorage::load_object(
	SharedObject* origin,
	hz::string<Allocator> path,
	int fd) {
	Elf_Ehdr ehdr;
	if (!try_read(fd, &ehdr, sizeof(ehdr)) ||
		ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
		ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
		ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
		ehdr.e_ident[EI_MAG3] != ELFMAG3 ||
		ehdr.e_ident[EI_DATA] != ELFDATA2LSB ||
		ehdr.e_ident[EI_VERSION] != EV_CURRENT
		) {
		return hz::error(LoadError::InvalidElf);
	}
	else if (ehdr.e_ident[EI_CLASS] != ELF_CLASS ||
		ehdr.e_machine != ELF_MACHINE) {
		return hz::error(LoadError::InvalidArch);
	}

	if (!try_seek(fd, static_cast<off64_t>(ehdr.e_phoff), SEEK_SET)) {
		return hz::error(LoadError::InvalidElf);
	}

	hz::vector<uint8_t, Allocator> phdrs {Allocator {}};
	phdrs.resize(ehdr.e_phnum * ehdr.e_phentsize);
	if (!try_read(fd, phdrs.data(), ehdr.e_phnum * ehdr.e_phentsize)) {
		return hz::error(LoadError::InvalidElf);
	}

	uintptr_t base = 0;
	uintptr_t end = 0;
	uintptr_t dynamic_offset = 0;
	bool base_found = false;
	for (int i = 0; i < ehdr.e_phnum; ++i) {
		auto* phdr = reinterpret_cast<const Elf_Phdr*>(phdrs.data() + i * ehdr.e_phentsize);
		if (phdr->p_type == PT_LOAD) {
			if (!base_found) {
				base = phdr->p_vaddr;
				base_found = true;
			}
			end = hz::max(end, phdr->p_vaddr + phdr->p_memsz);
		}
		else if (phdr->p_type == PT_DYNAMIC) {
			dynamic_offset = phdr->p_vaddr;
		}
	}

	__ensure(base == 0);

	uintptr_t total_size = (end - base + 0x1000 - 1) & ~(0x1000 - 1);
	void* load_base_ptr;
	if (sys_mmap(nullptr, total_size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0, &load_base_ptr) != 0) {
		return hz::error(LoadError::NoMemory);
	}
	auto load_base = reinterpret_cast<uintptr_t>(load_base_ptr);

	for (int i = 0; i < ehdr.e_phnum; ++i) {
		auto* phdr = reinterpret_cast<const Elf_Phdr*>(phdrs.data() + i * ehdr.e_phentsize);
		if (phdr->p_type == PT_LOAD) {
			void* tmp;
			__ensure(sys_mmap(
				reinterpret_cast<void*>(load_base + (phdr->p_vaddr & ~(0x1000 - 1))),
				(phdr->p_memsz + phdr->p_vaddr % 0x1000 + 0x1000 - 1) & ~(0x1000 - 1),
				PROT_READ | PROT_WRITE | PROT_EXEC,
				MAP_PRIVATE | MAP_ANON | MAP_FIXED,
				-1,
				0,
				&tmp) == 0);

			if (!try_seek(fd, static_cast<off64_t>(phdr->p_offset), SEEK_SET) ||
				!try_read(
					fd,
					reinterpret_cast<void*>(load_base + phdr->p_vaddr),
					phdr->p_filesz)) {
				__ensure(sys_munmap(load_base_ptr, total_size) == 0);
				return hz::error(LoadError::InvalidElf);
			}
			rtld_memset(
				reinterpret_cast<void*>(load_base + phdr->p_vaddr + phdr->p_filesz),
				0,
				phdr->p_memsz - phdr->p_filesz);
		}
	}

	auto* storage = Allocator::allocate(sizeof(SharedObject));
	auto* object = new (storage) SharedObject {
		origin,
		hz::string<Allocator> {Allocator {}},
		std::move(path),
		load_base,
		reinterpret_cast<const Elf_Dyn*>(load_base + dynamic_offset),
		reinterpret_cast<const Elf_Phdr*>(phdrs.data()),
		ehdr.e_phentsize,
		ehdr.e_phnum
	};

	return hz::success<SharedObject*>(object);
}

hz::result<SharedObject*, LoadError>
ObjectStorage::load_object_at_path(SharedObject* origin, hz::string<Allocator> path) {
	int fd;
	if (sys_openat(AT_FDCWD, path.data(), O_RDONLY, 0, &fd) != 0) {
		return hz::error(LoadError::NotFound);
	}
	else {
		UniqueFd unique {fd};
		return load_object(origin, std::move(path), unique);
	}
}

hz::result<SharedObject*, LoadError> ObjectStorage::load_object_with_name(SharedObject* origin, hz::string_view name) {
	for (auto& run_path : origin->run_paths) {
		int dir_fd;
		if (sys_openat(AT_FDCWD, run_path.data(), O_DIRECTORY, 0, &dir_fd) != 0) {
			continue;
		}

		int fd;
		if (sys_openat(dir_fd, name.data(), O_RDONLY, 0, &fd) != 0) {
			continue;
		}

		__ensure(sys_close(dir_fd) == 0);

		UniqueFd unique {fd};
		hz::string<Allocator> path {Allocator {}};
		path = run_path;
		if (!path.as_view().ends_with("/")) {
			path += '/';
		}
		path += name;

		if (auto result = load_object(origin, std::move(path), unique)) {
			return hz::success<SharedObject*>(result.value());
		}
	}

	for (auto& system_path : SYSTEM_LIBRARY_PATHS) {
		int dir_fd;
		if (sys_openat(AT_FDCWD, system_path.data(), O_DIRECTORY, 0, &dir_fd) != 0) {
			continue;
		}

		int fd;
		if (sys_openat(dir_fd, name.data(), O_RDONLY, 0, &fd) != 0) {
			continue;
		}

		__ensure(sys_close(dir_fd) == 0);

		UniqueFd unique {fd};
		hz::string<Allocator> path {Allocator {}};
		path = system_path;
		if (!path.as_view().ends_with("/")) {
			path += '/';
		}
		path += name;

		if (auto result = load_object(origin, std::move(path), unique)) {
			return hz::success<SharedObject*>(result.value());
		}
	}

	return hz::error(LoadError::NotFound);
}

namespace {
	bool LIBC_INITIALIZED = false;
}

LoadError ObjectStorage::load_dependencies(SharedObject* object, bool global) {
	hz::vector<SharedObject*, Allocator> load_list {Allocator {}};
	load_list.push_back(object);

	auto start = object;
	if (global) {
		global_scope.push_back(object);
	}

	auto tls_size_at_start = initial_tls_size;

	while (!load_list.empty()) {
		object = load_list.back();
		load_list.pop_back();

		init_list.push_back(object);

		if (object->tls_size && object->initial_tls_model) {
			auto offset = (initial_tls_size + object->tls_size + object->tls_align - 1)
			              & ~(object->tls_align - 1);
			if (total_initial_tls_size && offset > total_initial_tls_size) {
				println("rtld: failed to load object ", object->name, " (ran out of initial tls space)");

				for (auto obj : init_list) {
					auto base = obj->base;
					auto size = obj->end - obj->base;
					bool rtld_loaded = obj->rtld_loaded;
					obj->~SharedObject();
					if (rtld_loaded) {
						__ensure(sys_munmap(reinterpret_cast<void*>(base), size) == 0);
						Allocator::deallocate(obj);
					}
				}

				if (global) {
					while (!global_scope.empty()) {
						if (global_scope.back() == start) {
							global_scope.pop_back();
							break;
						}
						else {
							global_scope.pop_back();
						}
					}
				}

				initial_tls_size = tls_size_at_start;
				return LoadError::NoMemory;
			}

			object->tls_offset = offset;
			initial_tls_size = offset;
		}

		for (auto* dyn = object->dynamic; dyn->d_tag != DT_NULL; ++dyn) {
			if (dyn->d_tag != DT_NEEDED) {
				continue;
			}

			hz::string_view name {object->strtab + dyn->d_un.d_ptr};

			hz::result<SharedObject*, LoadError> result;
			if (name.find('/') != hz::string_view::npos) {
				bool already_loaded = false;
				for (auto added_object : objects) {
					if (added_object->path == name) {
						already_loaded = true;
						break;
					}
				}

				if (already_loaded) {
					continue;
				}

				println("rtld: loading ", name);

				hz::string<Allocator> path {Allocator {}};
				path = name;
				result = load_object_at_path(object, std::move(path));
			}
			else {
				bool already_loaded = false;
				for (auto added_object : objects) {
					if (added_object->name == name) {
						already_loaded = true;
						break;
					}
				}

				if (already_loaded) {
					continue;
				}

				println("rtld: loading ", name);

				result = load_object_with_name(object, name);
			}

			if (!result) {
				println("rtld: failed to load object ", name, " (needed by ", object->name, ")");

				for (auto obj : init_list) {
					auto base = obj->base;
					auto size = obj->end - obj->base;
					bool rtld_loaded = obj->rtld_loaded;
					obj->~SharedObject();
					if (rtld_loaded) {
						__ensure(sys_munmap(reinterpret_cast<void*>(base), size) == 0);
						Allocator::deallocate(obj);
					}
				}

				if (global) {
					while (!global_scope.empty()) {
						if (global_scope.back() == start) {
							global_scope.pop_back();
							break;
						}
						else {
							global_scope.pop_back();
						}
					}
				}

				initial_tls_size = tls_size_at_start;

				return result.error();
			}
			__ensure(result);
			auto* new_object = result.value();
			add_object(new_object);
			object->dependencies.push_back(new_object);
			if (global) {
				global_scope.push_back(new_object);
			}
			load_list.push_back(new_object);
		}
	}

	if (!LIBC_INITIALIZED) {
		objects[1]->relocate();
	}

	for (auto obj : init_list) {
		obj->relocate();
	}

	if (!LIBC_INITIALIZED) {
		objects[1]->late_relocate();
	}

	for (auto obj : init_list) {
		obj->late_relocate();
	}

	if (!LIBC_INITIALIZED) {
		destruct_list.push_back(objects[1]);
		objects[1]->run_init();

		if (objects[0]->preinit_array) {
			for (auto* fn = objects[0]->preinit_array; fn != objects[0]->preinit_array_end; ++fn) {
				(*fn)();
			}
		}
		LIBC_INITIALIZED = true;
	}

	return LoadError::Success;
}

hz::optional<ObjectSymbol> ObjectStorage::lookup(SharedObject* local, const char* name, LookupPolicy policy) {
	hz::optional<ObjectSymbol> res;

	if (local) {
		if (policy == LookupPolicy::LocalAndDeps) {
			hz::vector<SharedObject*, Allocator> stack {Allocator {}};
			struct Marker {};
			hz::unordered_map<SharedObject*, Marker, Allocator> visited {Allocator {}};

			stack.push_back(local);
			visited.insert(local, Marker {});
			for (size_t i = 0; i < stack.size(); ++i) {
				auto object = stack[i];

				if (auto sym = object->lookup(name);
					sym && sym->st_shndx != SHN_UNDEF && ELF_ST_BIND(sym->st_info) != STB_LOCAL) {
					if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
						res = ObjectSymbol {
							.object = object,
							.sym = sym
						};
					}
					else if (ELF_ST_BIND(sym->st_info) == STB_GNU_UNIQUE) {
						if (auto existing = unique_map.get(hz::string_view {name})) {
							return *existing;
						}
						hz::string<Allocator> key {Allocator {}};
						key = name;

						ObjectSymbol obj_sym {
							.object = object,
							.sym = sym
						};
						unique_map.insert(std::move(key), obj_sym);
						return obj_sym;
					}
					else {
						return {ObjectSymbol {
							.object = object,
							.sym = sym
						}};
					}
				}

				for (auto dep : object->dependencies) {
					if (visited.get(dep)) {
						continue;
					}
					stack.push_back(dep);
					visited.insert(dep, Marker {});
				}
			}

			return res;
		}

		if (policy != LookupPolicy::IgnoreLocal) {
			if (auto sym = local->lookup(name);
				sym && sym->st_shndx != SHN_UNDEF && ELF_ST_BIND(sym->st_info) != STB_LOCAL) {
				if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
					res = ObjectSymbol {
						.object = local,
						.sym = sym
					};
				}
				else if (ELF_ST_BIND(sym->st_info) == STB_GNU_UNIQUE) {
					if (auto existing = unique_map.get(hz::string_view {name})) {
						return *existing;
					}
					hz::string<Allocator> key {Allocator {}};
					key = name;

					ObjectSymbol obj_sym {
						.object = local,
						.sym = sym
					};
					unique_map.insert(std::move(key), obj_sym);
					return obj_sym;
				}
				else {
					return {ObjectSymbol {
						.object = local,
						.sym = sym
					}};
				}
			}
		}

		for (auto& dep : local->local_scope) {
			if (auto sym = dep->lookup(name);
				sym && sym->st_shndx != SHN_UNDEF && ELF_ST_BIND(sym->st_info) != STB_LOCAL) {
				if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
					res = ObjectSymbol {
						.object = dep,
						.sym = sym
					};
				}
				else if (ELF_ST_BIND(sym->st_info) == STB_GNU_UNIQUE) {
					if (auto existing = unique_map.get(hz::string_view {name})) {
						return *existing;
					}
					hz::string<Allocator> key {Allocator {}};
					key = name;

					ObjectSymbol obj_sym {
						.object = dep,
						.sym = sym
					};
					unique_map.insert(std::move(key), obj_sym);
					return obj_sym;
				}
				else {
					return {ObjectSymbol {
						.object = dep,
						.sym = sym
					}};
				}
			}
		}
	}

	for (auto object : global_scope) {
		if (object == local) {
			continue;
		}

		if (auto sym = object->lookup(name);
			sym && sym->st_shndx != SHN_UNDEF && ELF_ST_BIND(sym->st_info) != STB_LOCAL) {
			if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
				res = ObjectSymbol {
					.object = object,
					.sym = sym
				};
			}
			else if (ELF_ST_BIND(sym->st_info) == STB_GNU_UNIQUE) {
				if (auto existing = unique_map.get(hz::string_view {name})) {
					return *existing;
				}
				hz::string<Allocator> key {Allocator {}};
				key = name;

				ObjectSymbol obj_sym {
					.object = object,
					.sym = sym
				};
				unique_map.insert(std::move(key), obj_sym);
				return obj_sym;
			}
			else {
				return {ObjectSymbol {
					.object = object,
					.sym = sym
				}};
			}
		}
	}

	return res;
}

void ObjectStorage::init_objects() {
	for (size_t i = init_list.size(); i > 0; --i) {
		if (!init_list[i - 1]->initialized) {
			destruct_list.push_back(init_list[i - 1]);
			init_list[i - 1]->run_init();
			init_list[i - 1]->initialized = true;
		}
	}
	init_list.clear();
}

void ObjectStorage::init_tls(void* tcb) {
	for (auto object : objects) {
		if (!object->initial_tls_model) {
			continue;
		}

		char* ptr = static_cast<char*>(tcb) - object->tls_offset;
		rtld_memcpy(ptr, object->tls_image, object->tls_image_size);
		rtld_memset(ptr + object->tls_image_size, 0, object->tls_size - object->tls_image_size);
	}
}

void ObjectStorage::destruct_objects() {
	__ensure(destruct_list.size() == objects.size());

	for (size_t i = destruct_list.size(); i > 0; --i) {
		__ensure(!destruct_list[i - 1]->destructed);
		destruct_list[i - 1]->run_fini();
		destruct_list[i - 1]->destructed = true;
	}

	for (auto object : objects) {
		object->~SharedObject();
		if (object->rtld_loaded) {
			__ensure(sys_munmap(reinterpret_cast<void*>(object->base), object->end - object->base) == 0);
			Allocator::deallocate(object);
		}
	}

	objects.clear();
	objects.shrink_to_fit();
}

hz::spinlock<hz::manually_init<ObjectStorage>> OBJECT_STORAGE;
