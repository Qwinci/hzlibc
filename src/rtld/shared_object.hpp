#pragma once
#include <stdint.h>
#include "elf.hpp"
#include "allocator.hpp"
#include <hz/string.hpp>
#include <hz/vector.hpp>

enum class RtState {
	Consistent,
	Add,
	Delete
};

struct LinkMap {
	uintptr_t base;
	const char* name;
	const Elf_Dyn* dynamic;
	LinkMap* next;
	LinkMap* prev;
};

struct DebugInterface {
	int version;
	LinkMap* head;
	void (*brk)();
	RtState state;
	void* ld_base;
};

struct TlsdescData {
	uintptr_t index;
	uintptr_t addend;
};

struct SharedObject {
	SharedObject(
		SharedObject* origin,
		hz::string<Allocator> name,
		hz::string<Allocator> path,
		uintptr_t base,
		Elf_Dyn* dynamic,
		const Elf_Phdr* phdrs,
		uint16_t phent,
		uint16_t phnum);

	~SharedObject();

	void relocate();
	void late_relocate();

	void run_init();
	void run_fini();

	const Elf_Sym* lookup(const char* sym_name);

	uintptr_t tls_size {};
	uintptr_t tls_align {};
	void* tls_image {};
	uintptr_t tls_image_size {};
	uintptr_t num_symbols {};
	hz::string<Allocator> name;
	hz::string<Allocator> path;
	hz::vector<hz::string<Allocator>, Allocator> run_paths;
	hz::vector<SharedObject*, Allocator> local_scope {Allocator {}};
	hz::vector<SharedObject*, Allocator> dependencies {Allocator {}};
	hz::vector<Elf_Phdr, Allocator> phdrs_vec;
	hz::vector<TlsdescData*, Allocator> tls_descs {Allocator {}};

	uintptr_t tls_offset {};
	uintptr_t tls_module_index {};

	const Elf_Sym* elf_lookup(const char* sym_name) const;
	const Elf_Sym* gnu_lookup(const char* sym_name) const;

	uintptr_t base;
	uintptr_t end;
	const Elf_Dyn* dynamic;
	const uint32_t* hashtab {};
	const uint32_t* gnutab {};
	const char* strtab {};
	const Elf_Sym* symtab {};

	using InitFn = void (*)();
	InitFn init {};
	InitFn fini {};
	InitFn* preinit_array {};
	InitFn* preinit_array_end {};
	InitFn* init_array {};
	InitFn* init_array_end {};
	InitFn* fini_array {};
	InitFn* fini_array_end {};

	LinkMap link_map {};

	bool symbolic_resolution {};
	bool initial_tls_model {};
	bool tls_initialized {};
	bool rtld_loaded {true};
	bool initialized {};
	bool destructed {};
};
