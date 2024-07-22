#pragma once
#include <stdint.h>
#include "elf.hpp"
#include "allocator.hpp"
#include <hz/string.hpp>
#include <hz/vector.hpp>

struct SharedObject {
	SharedObject(
		SharedObject* origin,
		hz::string<Allocator> name,
		hz::string<Allocator> path,
		uintptr_t base,
		const Elf_Dyn* dynamic,
		const Elf_Phdr* phdrs,
		uint16_t phent,
		uint16_t phnum);

	void relocate();
	void late_relocate();

	void run_init();
	void run_fini();

	const Elf_Sym* lookup(const char* sym_name);

	uintptr_t tls_size {};
	uintptr_t tls_align {};
	uintptr_t tls_offset {};
	void* tls_image {};
	uintptr_t tls_image_size {};
	uintptr_t num_symbols {};
	hz::string<Allocator> name;
	hz::string<Allocator> path;
	hz::vector<hz::string<Allocator>, Allocator> run_paths;
	hz::vector<SharedObject*, Allocator> local_scope {Allocator {}};
	hz::vector<SharedObject*, Allocator> dependencies {Allocator {}};
	hz::vector<Elf_Phdr, Allocator> phdrs_vec;

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

	bool symbolic_resolution {};
	bool initial_tls_model {};
	bool rtld_loaded {true};
	bool initialized {};
	bool destructed {};
};
