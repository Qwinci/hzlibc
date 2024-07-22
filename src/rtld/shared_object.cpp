#include "shared_object.hpp"
#include "log.hpp"
#include "object_storage.hpp"

#define memcpy __builtin_memcpy

SharedObject::SharedObject(
	SharedObject* origin,
	hz::string<Allocator> name,
	hz::string<Allocator> new_path,
	uintptr_t base,
	const Elf_Dyn* dynamic,
	const Elf_Phdr* phdrs,
	uint16_t phent,
	uint16_t phnum)
		: name {std::move(name)}, path {std::move(new_path)}, run_paths {Allocator {}},
		phdrs_vec {Allocator {}}, base {base}, end {}, dynamic {dynamic} {

	uintptr_t link_start = 0;
	uintptr_t link_end = 0;
	bool link_start_found = false;
	for (int i = 0; i < phnum; ++i) {
		auto* phdr = reinterpret_cast<Elf_Phdr*>(reinterpret_cast<uintptr_t>(phdrs) + i * phent);
		if (phdr->p_type == PT_LOAD) {
			if (!link_start_found) {
				link_start = phdr->p_vaddr;
				link_start_found = true;
			}
			link_end = hz::max(link_end, phdr->p_vaddr + phdr->p_memsz);
		}
		else if (phdr->p_type == PT_TLS) {
			tls_size = phdr->p_memsz;
			tls_align = phdr->p_align;
			tls_image = reinterpret_cast<void*>(base + phdr->p_vaddr);
			tls_image_size = phdr->p_filesz;
		}
		phdrs_vec.push_back(*phdr);
	}

	end = (base + (link_end - link_start) + 0x1000 - 1) & ~(0x1000 - 1);

	uintptr_t run_path_offset = 0;
	uintptr_t so_name_offset = 0;
	uintptr_t init_array_size = 0;
	uintptr_t fini_array_size = 0;
	uintptr_t preinit_array_size = 0;
	for (auto* dyn = dynamic; dyn->d_tag != DT_NULL; ++dyn) {
		switch (dyn->d_tag) {
			case DT_HASH:
				hashtab = reinterpret_cast<const uint32_t*>(base + dyn->d_un.d_ptr);
				break;
			case DT_STRTAB:
				strtab = reinterpret_cast<const char*>(base + dyn->d_un.d_ptr);
				break;
			case DT_SYMTAB:
				symtab = reinterpret_cast<const Elf_Sym*>(base + dyn->d_un.d_ptr);
				break;
			case DT_INIT:
				init = reinterpret_cast<InitFn>(base + dyn->d_un.d_ptr);
				break;
			case DT_SONAME:
				so_name_offset = dyn->d_un.d_ptr;
				break;
			case DT_SYMBOLIC:
				symbolic_resolution = true;
				break;
			case DT_INIT_ARRAY:
				init_array = reinterpret_cast<InitFn*>(base + dyn->d_un.d_ptr);
				break;
			case DT_FINI_ARRAY:
				fini_array = reinterpret_cast<InitFn*>(base + dyn->d_un.d_ptr);
				break;
			case DT_INIT_ARRAYSZ:
				init_array_size = dyn->d_un.d_val;
				break;
			case DT_FINI_ARRAYSZ:
				fini_array_size = dyn->d_un.d_val;
				break;
			case DT_RUNPATH:
				run_path_offset = dyn->d_un.d_ptr;
				break;
			case DT_FLAGS:
				if (dyn->d_un.d_val & DF_STATIC_TLS) {
					initial_tls_model = true;
				}
				if (dyn->d_un.d_val & DF_SYMBOLIC) {
					symbolic_resolution = true;
				}
				break;
			case DT_PREINIT_ARRAY:
				preinit_array = reinterpret_cast<InitFn*>(base + dyn->d_un.d_ptr);
				break;
			case DT_PREINIT_ARRAYSZ:
				preinit_array_size = dyn->d_un.d_val;
				break;
			case DT_GNU_HASH:
				gnutab = reinterpret_cast<const uint32_t*>(base + dyn->d_un.d_ptr);
				break;
		}
	}

	if (init_array_size) {
		init_array_end = init_array + init_array_size / sizeof(InitFn);
	}
	if (fini_array_size) {
		fini_array_end = fini_array + fini_array_size / sizeof(InitFn);
	}
	if (preinit_array_size) {
		preinit_array_end = preinit_array + preinit_array_size / sizeof(InitFn);
	}

	if (so_name_offset && this->name.empty()) {
		this->name = strtab + so_name_offset;
	}

	if (gnutab) {
		auto num_buckets = gnutab[0];
		auto sym_offset = gnutab[1];
		auto bloom_size = gnutab[2];
		auto* bloom = reinterpret_cast<const Elf_Bloom*>(&gnutab[4]);
		auto* buckets = reinterpret_cast<const uint32_t*>(&bloom[bloom_size]);
		auto* chain = &buckets[num_buckets];

		uint32_t sym_index = 0;
		for (uint32_t i = 0; i < num_buckets; ++i) {
			sym_index = hz::max(sym_index, buckets[i]);
		}

		if (sym_index) {
			while ((chain[sym_index - sym_offset] & 1) == 0) {
				++sym_index;
			}
		}

		num_symbols = sym_index + 1;
	}
	else {
		__ensure(hashtab);
		num_symbols = hashtab[1];
	}

	if (run_path_offset) {
		hz::string_view orig_run_path {strtab + run_path_offset};

		size_t pos = 0;
		while (pos < orig_run_path.size()) {
			size_t segment_end = orig_run_path.find(':', pos);
			auto segment = orig_run_path.substr(pos, segment_end - pos);

			hz::string<Allocator> run_path {Allocator {}};

			size_t last_origin_pos = 0;
			size_t segment_pos = 0;
			while ((segment_pos = segment.find("$ORIGIN", segment_pos)) != hz::string_view::npos) {
				run_path += segment.substr(last_origin_pos, segment_pos - last_origin_pos);
				if (origin) {
					run_path += origin->path;
				}
				else {
					run_path += path.as_view().substr(0, path.as_view().find_last_of("/"));
				}

				segment_pos += 7;
				last_origin_pos = segment_pos;
			}

			if (last_origin_pos) {
				run_path += segment.substr(last_origin_pos);
			}
			else {
				run_path = segment;
			}

			run_paths.push_back(std::move(run_path));

			if (segment_end == hz::string_view::npos) {
				break;
			}
			else {
				pos = segment_end + 1;
			}
		}
	}
}

void SharedObject::relocate() {
	uintptr_t rela = 0;
	uintptr_t relr = 0;
	uintptr_t rel = 0;
	uintptr_t rela_size = 0;
	uintptr_t relr_size = 0;
	uintptr_t rel_size = 0;
	uintptr_t plt_rel_size = 0;
	uintptr_t plt_rel_type = 0;
	uintptr_t plt_rel = 0;

	for (auto* dyn = dynamic; dyn->d_tag != DT_NULL; ++dyn) {
		switch (dyn->d_tag) {
			case DT_PLTRELSZ:
				plt_rel_size = dyn->d_un.d_val;
				break;
			case DT_RELA:
				rela = base + dyn->d_un.d_ptr;
				break;
			case DT_RELASZ:
				rela_size = dyn->d_un.d_val;
				break;
			case DT_REL:
				rel = base + dyn->d_un.d_ptr;
				break;
			case DT_RELSZ:
				rel_size = dyn->d_un.d_val;
				break;
			case DT_PLTREL:
				plt_rel_type = dyn->d_un.d_val;
				break;
			case DT_JMPREL:
				plt_rel = base + dyn->d_un.d_ptr;
				break;
			case DT_RELRSZ:
				relr_size = dyn->d_un.d_val;
				break;
			case DT_RELR:
				relr = base + dyn->d_un.d_ptr;
				break;
			default:
				break;
		}
	}

	bool has_unresolved_symbols = false;

	for (uintptr_t i = 0; i < rela_size; i += sizeof(Elf_Rela)) {
		auto entry = *reinterpret_cast<Elf_Rela*>(rela + i);
		auto type = ELF_R_TYPE(entry.r_info);

		if (type == R_COPY || type == R_IRELATIVE) {
			continue;
		}

		auto* ptr = reinterpret_cast<uintptr_t*>(base + entry.r_offset);

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			resolved_sym = OBJECT_STORAGE.get_unsafe()->lookup(symbolic_resolution ? this : nullptr, sym_name, LookupPolicy::None);
		}

		switch (type) {
			case R_RELATIVE:
				*ptr = base + entry.r_addend;
				break;
			case R_ABSOLUTE:
			case R_GLOB_DAT:
			{
				if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
					if (resolved_sym) {
						*ptr = resolved_sym->object->base + resolved_sym->sym->st_value + entry.r_addend;
					}
					else {
						println("rtld: unresolved weak symbol ", strtab + sym->st_name, " in object ", name);
						*ptr = 0;
					}
				}
				else {
					if (!resolved_sym) {
						println("rtld: unresolved symbol ", strtab + sym->st_name, " in object ", name);
						has_unresolved_symbols = true;
					}
					else {
						*ptr = resolved_sym->object->base + resolved_sym->sym->st_value + entry.r_addend;
					}
				}
				break;
			}
			case R_TPOFF:
				*ptr = -tls_offset + entry.r_addend;
				break;
			default:
				panic("unsupported relocation type ", type);
		}
	}

	uintptr_t* relr_addr = nullptr;
	for (uintptr_t i = 0; i < relr_size; i += sizeof(Elf_Relr)) {
		auto entry = *reinterpret_cast<Elf_Relr*>(relr + i);

		// even entry indicates the starting address
		if (!(entry & 1)) {
			relr_addr = reinterpret_cast<uintptr_t*>(base + entry);
			*relr_addr++ += base;
		}
		// odd entry indicates a bitmap of locations to be relocated
		else {
			__ensure(relr_addr);
			for (int j = 0; entry; ++j) {
				if (entry & 1) {
					relr_addr[j] += base;
				}
				entry >>= 1;
			}
			// each entry describes at max 63 or 31 locations
			relr_addr += 8 * sizeof(Elf_Relr) - 1;
		}
	}

	for (uintptr_t i = 0; i < rel_size; i += sizeof(Elf_Rel)) {
		auto entry = *reinterpret_cast<Elf_Rel*>(rel + i);
		auto type = ELF_R_TYPE(entry.r_info);

		if (type == R_COPY || type == R_IRELATIVE) {
			continue;
		}

		auto* ptr = reinterpret_cast<uintptr_t*>(base + entry.r_offset);

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			resolved_sym = OBJECT_STORAGE.get_unsafe()->lookup(symbolic_resolution ? this : nullptr, sym_name, LookupPolicy::None);
		}

		switch (type) {
			case R_RELATIVE:
				*ptr += base;
				break;
			case R_ABSOLUTE:
			case R_GLOB_DAT:
			{
				if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
					if (resolved_sym) {
						*ptr = resolved_sym->object->base + resolved_sym->sym->st_value;
					}
					else {
						println("rtld: unresolved weak symbol ", strtab + sym->st_name, " in object ", name);
						*ptr = 0;
					}
				}
				else {
					if (!resolved_sym) {
						println("rtld: unresolved symbol ", strtab + sym->st_name, " in object ", name);
						has_unresolved_symbols = true;
					}
					else {
						*ptr = resolved_sym->object->base + resolved_sym->sym->st_value;
					}
				}
				break;
			}
			case R_TPOFF:
				*ptr += -tls_offset;
				break;
			default:
				panic("unsupported relocation type ", type);
		}
	}

	if (plt_rel_type == DT_RELA) {
		for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rela)) {
			auto entry = *reinterpret_cast<Elf_Rela*>(plt_rel + i);
			auto type = ELF_R_TYPE(entry.r_info);
			auto* ptr = reinterpret_cast<uintptr_t*>(base + entry.r_offset);

			hz::optional<ObjectSymbol> resolved_sym;
			auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
			if (ELF_R_SYM(entry.r_info)) {
				const char* sym_name = strtab + sym->st_name;
				resolved_sym = OBJECT_STORAGE.get_unsafe()->lookup(symbolic_resolution ? this : nullptr, sym_name, LookupPolicy::None);
			}

			switch (type) {
				case R_JUMP_SLOT:
				{
					if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
						if (resolved_sym) {
							*ptr = resolved_sym->object->base + resolved_sym->sym->st_value + entry.r_addend;
						}
						else {
							println("rtld: unresolved weak symbol ", strtab + sym->st_name, " in object ", name);
							*ptr = 0;
						}
					}
					else {
						if (!resolved_sym) {
							println("rtld: unresolved symbol ", strtab + sym->st_name, " in object ", name);
							has_unresolved_symbols = true;
						}
						else {
							*ptr = resolved_sym->object->base + resolved_sym->sym->st_value + entry.r_addend;
						}
					}
					break;
				}
				default:
					panic("unsupported plt relocation type ", type);
			}
		}
	}
	else {
		for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rel)) {
			auto entry = *reinterpret_cast<Elf_Rel*>(plt_rel + i);
			auto type = ELF_R_TYPE(entry.r_info);
			auto* ptr = reinterpret_cast<uintptr_t*>(base + entry.r_offset);

			hz::optional<ObjectSymbol> resolved_sym;
			auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
			if (ELF_R_SYM(entry.r_info)) {
				const char* sym_name = strtab + sym->st_name;
				resolved_sym = OBJECT_STORAGE.get_unsafe()->lookup(symbolic_resolution ? this : nullptr, sym_name, LookupPolicy::None);
			}

			switch (type) {
				case R_JUMP_SLOT:
				{
					if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
						if (resolved_sym) {
							*ptr = resolved_sym->object->base + resolved_sym->sym->st_value;
						}
						else {
							println("rtld: unresolved weak symbol ", strtab + sym->st_name, " in object ", name);
							*ptr = 0;
						}
					}
					else {
						if (!resolved_sym) {
							println("rtld: unresolved symbol ", strtab + sym->st_name, " in object ", name);
							has_unresolved_symbols = true;
						}
						else {
							*ptr = resolved_sym->object->base + resolved_sym->sym->st_value;
						}
					}
					break;
				}
				default:
					panic("unsupported plt relocation type ", type);
			}
		}
	}

	if (has_unresolved_symbols) {
		panic("object had unresolved symbols");
	}
}

void SharedObject::late_relocate() {
	uintptr_t rela = 0;
	uintptr_t rela_size = 0;

	for (auto* dyn = dynamic; dyn->d_tag != DT_NULL; ++dyn) {
		switch (dyn->d_tag) {
			case DT_RELA:
				rela = base + dyn->d_un.d_ptr;
				break;
			case DT_RELASZ:
				rela_size = dyn->d_un.d_val;
				break;
			default:
				break;
		}
	}

	for (uintptr_t i = 0; i < rela_size; i += sizeof(Elf_Rela)) {
		auto entry = *reinterpret_cast<Elf_Rela*>(rela + i);
		auto type = ELF_R_TYPE(entry.r_info);

		if (type != R_COPY && type != R_IRELATIVE) {
			continue;
		}

		auto* ptr = reinterpret_cast<uintptr_t*>(base + entry.r_offset);

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			resolved_sym = OBJECT_STORAGE.get_unsafe()->lookup(this, sym_name, LookupPolicy::IgnoreLocal);
		}

		switch (type) {
			case R_COPY:
			{
				if (!resolved_sym) {
					println("rtld: unresolved symbol ", strtab + sym->st_name, " in object ", name);
				}
				__ensure(resolved_sym && "unresolved symbol for copy relocation");
				memcpy(
					ptr,
					reinterpret_cast<const void*>(resolved_sym->object->base + resolved_sym->sym->st_value),
					resolved_sym->sym->st_size);
				break;
			}
#if defined(__x86_64__) || defined(__i386__)
			case R_IRELATIVE:
			{
				auto addr = base + entry.r_addend;
				auto* fn = reinterpret_cast<uintptr_t (*)()>(addr);
				*ptr = fn();
				break;
			}
#elif defined(__aarch64__)
			case R_IRELATIVE:
			{
				uintptr_t addr = base + entry.r_addend;
				auto* fn = reinterpret_cast<uintptr_t (*)(uint64_t)>(addr);
				// todo pass AT_HWCAP
				*ptr = fn(0);
				break;
			}
#endif
			default:
				panic("unsupported late relocation type ", type);
		}
	}
}

void SharedObject::run_init() {
	if (init) {
		init();
	}
	for (auto* fn = init_array; fn != init_array_end; ++fn) {
		(*fn)();
	}
}

void SharedObject::run_fini() {
	if (destructed) {
		return;
	}

	size_t size = fini_array_end - fini_array;
	for (size_t i = size; i > 0; --i) {
		fini_array[i - 1]();
	}
	if (fini) {
		fini();
	}
	destructed = true;
}

const Elf_Sym* SharedObject::lookup(const char* sym_name) {
	if (gnutab) {
		return gnu_lookup(sym_name);
	}
	else {
		return elf_lookup(sym_name);
	}
}

namespace {
	uint32_t elf_hash(const char* name) {
		auto* s = reinterpret_cast<const unsigned char*>(name);
		uint32_t hash = 0;
		for (; *s; ++s) {
			hash = (hash << 4) + *s;
			hash ^= hash >> 24 & 0xF0;
		}
		return hash & 0xFFFFFFF;
	}

	uint32_t gnu_hash(const char* name) {
		auto* s = reinterpret_cast<const unsigned char*>(name);
		uint32_t hash = 5381;
		for (; *s; ++s) {
			hash = (hash << 5) + hash + *s;
		}
		return hash;
	}

	bool rtld_strcmp(const char* a, const char* b) {
		for (;; ++a, ++b) {
			if (*a != *b) {
				return false;
			}
			else if (!*a) {
				return true;
			}
		}
	}
}

const Elf_Sym* SharedObject::elf_lookup(const char* sym_name) const {
	auto hash = elf_hash(sym_name);

	auto num_buckets = hashtab[0];
	auto* buckets = &hashtab[2];
	auto* chain = &buckets[num_buckets];

	for (uint32_t i = buckets[hash % num_buckets]; i; i = chain[i]) {
		if (rtld_strcmp(sym_name, strtab + symtab[i].st_name)) {
			return &symtab[i];
		}
	}

	return nullptr;
}

const Elf_Sym* SharedObject::gnu_lookup(const char* sym_name) const {
	auto hash = gnu_hash(sym_name);

	auto num_buckets = gnutab[0];
	auto sym_offset = gnutab[1];
	auto bloom_size = gnutab[2];
	auto bloom_shift = gnutab[3];
	auto* bloom = reinterpret_cast<const Elf_Bloom*>(&gnutab[4]);
	auto* buckets = reinterpret_cast<const uint32_t*>(&bloom[bloom_size]);
	auto* chain = &buckets[num_buckets];

	auto word = bloom[hash / ELF_CLASS_BITS % bloom_size];
	auto mask = Elf_Bloom {1} << (hash % ELF_CLASS_BITS) |
	            Elf_Bloom {1} << ((hash >> bloom_shift) % ELF_CLASS_BITS);

	if ((word & mask) != mask) {
		return nullptr;
	}

	auto sym_index = buckets[hash % num_buckets];
	if (sym_index < sym_offset) {
		return nullptr;
	}

	for (;; ++sym_index) {
		auto* current_sym_name = strtab + symtab[sym_index].st_name;
		auto sym_hash = chain[sym_index - sym_offset];

		if ((hash | 1) == (sym_hash | 1) && rtld_strcmp(sym_name, current_sym_name)) {
			return &symtab[sym_index];
		}

		if (sym_hash & 1) {
			break;
		}
	}

	return nullptr;
}
