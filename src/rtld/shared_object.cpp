#include "shared_object.hpp"
#include "object_storage.hpp"
#include "opts.hpp"

#define memcpy __builtin_memcpy

extern DebugInterface DEBUG_INTERFACE;

SharedObject::SharedObject(
	SharedObject* origin,
	hz::string<Allocator> name,
	hz::string<Allocator> new_path,
	uintptr_t base,
	Elf_Dyn* dynamic,
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
		// if name.data() is null then this is the early libc object.
		if (this->name.data()) {
			phdrs_vec.push_back(*phdr);
		}
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
			case DT_DEBUG:
			{
				dyn->d_un.d_ptr = reinterpret_cast<uintptr_t>(&DEBUG_INTERFACE);
				break;
			}
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

	if (so_name_offset && this->name.data() && this->name.empty()) {
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

	link_map.base = base;
	link_map.name = this->path.data();
	link_map.dynamic = dynamic;
}


SharedObject::~SharedObject() {
	for (auto ptr : tls_descs) {
		delete ptr;
	}
}

#ifdef __x86_64__

asm(R"(
.pushsection .text
.hidden __tlsdesc_static
.type __tlsdesc_static, @function
__tlsdesc_static:
	mov 8(%rax), %rax
	ret

.hidden __tlsdesc_dynamic
.type __tlsdesc_dynamic, @function
__tlsdesc_dynamic:
	push %rbx
	push %rcx

	mov 8(%rax), %rax

	// index
	mov (%rax), %rbx
	// addend
	mov 8(%rax), %rcx

	// (*tp).dtv.data
	mov %fs:64, %rax
	// dtv.data[index]
	mov (%rax, %rbx, 8), %rax
	// + addend
	add %rcx, %rax
	sub %fs:0, %rax

	pop %rcx
	pop %rbx
	ret

.popsection
)");

#elif defined(__aarch64__)

asm(R"(
.pushsection .text
.hidden __tlsdesc_static
.type __tlsdesc_static, @function
__tlsdesc_static:
	ldr x0, [x0, #8]
	ret

.hidden __tlsdesc_dynamic
.type __tlsdesc_dynamic, @function
__tlsdesc_dynamic:
	stp x1, x2, [sp, #-16]!
	str x3, [sp, #-16]!

	ldr x0, [x0, #8]

	// index, addend
	ldp x1, x2, [x0]

	mrs x3, tpidr_el0
	// (*tp).dtv.data
	mov x0, #-16432
	add x0, x3, x0
	ldr x0, [x0, x1, lsl #3]

	// + addend
	add x0, x0, x2
	// - tp
	sub x0, x0, x3

	ldr x3, [sp], #16
	ldp x1, x2, [sp], #16
	ret

.popsection
)");

#endif

extern "C" void* __tlsdesc_static(void*);
extern "C" void* __tlsdesc_dynamic(void*);

void* access_tls_for_object(SharedObject* object);

void* rtld_memset(void* __restrict dest, int ch, size_t size);

namespace {
	struct Relocation {
		inline Relocation(SharedObject* object, Elf_Rela rela)
			: object {object}, rela {rela} {}

		inline Relocation(SharedObject* object, Elf_Rel rel)
			: object {object}, rela {} {
			rela.r_offset = rel.r_offset;
			rela.r_info = rel.r_info;
			rela.r_addend = *reinterpret_cast<intptr_t*>(object->base + rel.r_offset);
		}

		inline Relocation(SharedObject* object, Elf_Rel rel, intptr_t addend)
			: object {object}, rela {} {
			rela.r_offset = rel.r_offset;
			rela.r_info = rel.r_info;
			rela.r_addend = addend;
		}

		[[nodiscard]] inline uintptr_t rel_addend() const {
			return object->base + rela.r_addend;
		}

		inline void apply(uintptr_t value) const {
			*reinterpret_cast<uintptr_t*>(object->base + rela.r_offset) = value;
		}

		SharedObject* object;
		Elf_Rela rela;
	};
}

static bool process_relocation(Relocation reloc, hz::optional<ObjectSymbol> resolved_sym) {
	auto type = ELF_R_TYPE(reloc.rela.r_info);
	auto sym_index = ELF_R_SYM(reloc.rela.r_info);
	auto* sym = &reloc.object->symtab[sym_index];

	switch (type) {
		case R_RELATIVE:
			reloc.apply(reloc.rel_addend());
			break;
		case R_ABSOLUTE:
		{
			if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
				if (resolved_sym) {
					reloc.apply(resolved_sym->object->base + resolved_sym->sym->st_value + reloc.rela.r_addend);
				}
				else {
					if constexpr (LOG_MISSING_WEAK_SYMS) {
						println(
							"rtld: unresolved weak symbol ",
							reloc.object->strtab + sym->st_name,
							" in object ",
							reloc.object->name);
					}

					reloc.apply(0);
				}
			}
			else {
				if (!resolved_sym) {
					println(
						"rtld: unresolved symbol ",
						reloc.object->strtab + sym->st_name,
						" in object ",
						reloc.object->name);
					return false;
				}
				else {
					reloc.apply(resolved_sym->object->base + resolved_sym->sym->st_value + reloc.rela.r_addend);
				}
			}
			break;
		}
		case R_GLOB_DAT:
		case R_JUMP_SLOT:
		{
			if (ELF_ST_BIND(sym->st_info) == STB_WEAK) {
				if (resolved_sym) {
					reloc.apply(resolved_sym->object->base + resolved_sym->sym->st_value);
				}
				else {
					if constexpr (LOG_MISSING_WEAK_SYMS) {
						println(
							"rtld: unresolved weak symbol ",
							reloc.object->strtab + sym->st_name,
							" in object ",
							reloc.object->name);
					}

					reloc.apply(0);
				}
			}
			else {
				if (!resolved_sym) {
					println(
						"rtld: unresolved symbol ",
						reloc.object->strtab + sym->st_name,
						" in object ",
						reloc.object->name);
					return false;
				}
				else {
					reloc.apply(resolved_sym->object->base + resolved_sym->sym->st_value);
				}
			}
			break;
		}
		case R_TPOFF:
		{
			uintptr_t tls_offset;
			uintptr_t offset;
			if (sym_index) {
				__ensure(resolved_sym);
				__ensure(resolved_sym->object->initial_tls_model);
				tls_offset = resolved_sym->object->tls_offset;
				offset = resolved_sym->sym->st_value;
			}
			else {
				__ensure(reloc.object->initial_tls_model);
				tls_offset = reloc.object->tls_offset;
				offset = 0;
			}

			offset += reloc.rela.r_addend;

			if constexpr (TLS_ABOVE_TP) {
				reloc.apply(tls_offset + TLS_OFFSET_FROM_TP + offset);
			}
			else {
				reloc.apply(-tls_offset + TLS_OFFSET_FROM_TP + offset);
			}

			break;
		}
		case R_DTPMOD:
			if (sym_index) {
				__ensure(resolved_sym);
				reloc.apply(reinterpret_cast<uintptr_t>(resolved_sym->object));
			}
			else {
				reloc.apply(reinterpret_cast<uintptr_t>(reloc.object));
			}
			break;
		case R_DTPOFF:
			__ensure(resolved_sym);
			reloc.apply(resolved_sym->sym->st_value + reloc.rela.r_addend);
			break;
#ifdef R_TLSDESC
		case R_TLSDESC:
		{
			SharedObject* target;
			uintptr_t symbol_value = 0;
			if (sym_index) {
				if (!resolved_sym) {
					println(
						"rtld: unresolved tlsdesc symbol ",
						reloc.object->strtab + sym->st_name,
						" in object ",
						reloc.object->name);
					return false;
				}
				target = resolved_sym->object;
				symbol_value = resolved_sym->sym->st_value;
			}
			else {
				target = reloc.object;
			}

			auto ptr = reinterpret_cast<uintptr_t*>(reloc.object->base + reloc.rela.r_offset);
			if (target->initial_tls_model) {
				ptr[0] = reinterpret_cast<uintptr_t>(&__tlsdesc_static);
				if constexpr (TLS_ABOVE_TP) {
					ptr[1] = target->tls_offset + TLS_OFFSET_FROM_TP + symbol_value + reloc.rela.r_addend;
				}
				else {
					ptr[1] = -target->tls_offset + TLS_OFFSET_FROM_TP + symbol_value + reloc.rela.r_addend;
				}
			}
			else {
				auto* data = new TlsdescData {
					.index = target->tls_module_index,
					.addend = symbol_value + reloc.rela.r_addend
				};

				access_tls_for_object(target);

				ptr[0] = reinterpret_cast<uintptr_t>(&__tlsdesc_dynamic);
				ptr[1] = reinterpret_cast<uintptr_t>(data);
				reloc.object->tls_descs.push_back(data);
			}

			break;
		}
#endif
		default:
			panic("unsupported relocation type ", type);
	}

	return true;
}

static void process_late_relocation(Relocation reloc, hz::optional<ObjectSymbol> resolved_sym) {
	auto type = ELF_R_TYPE(reloc.rela.r_info);
	auto sym_index = ELF_R_SYM(reloc.rela.r_info);
	auto* sym = &reloc.object->symtab[sym_index];

	switch (type) {
		case R_COPY:
		{
			if (!resolved_sym) {
				println(
					"rtld: unresolved symbol ",
					reloc.object->strtab + sym->st_name,
					" in object ",
					reloc.object->name);
			}
			__ensure(resolved_sym && "unresolved symbol for copy relocation");

			auto ptr = reinterpret_cast<uintptr_t*>(reloc.object->base + reloc.rela.r_offset);
			memcpy(
				ptr,
				reinterpret_cast<const void*>(resolved_sym->object->base + resolved_sym->sym->st_value),
				resolved_sym->sym->st_size);
			break;
		}
#if defined(__x86_64__) || defined(__i386__)
		case R_IRELATIVE:
		{
			auto addr = reloc.rel_addend();
			auto* fn = reinterpret_cast<uintptr_t (*)()>(addr);
			reloc.apply(fn());
			break;
		}
#elif defined(__aarch64__)
			case R_IRELATIVE:
			{
				uintptr_t addr = reloc.rel_addend();
				auto* fn = reinterpret_cast<uintptr_t (*)(uint64_t)>(addr);
				// todo pass AT_HWCAP
				reloc.apply(fn(0));
				break;
			}
#endif
		default:
			panic("unsupported late relocation type ", type);
	}
}

intptr_t* SAVED_LIBC_REL_ADDENDS;
size_t SAVED_LIBC_REL_ADDEND_COUNT = 0;

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
	LookupPolicy lookup_policy = symbolic_resolution ? LookupPolicy::Symbolic : LookupPolicy::None;

	for (uintptr_t i = 0; i < rela_size; i += sizeof(Elf_Rela)) {
		auto entry = *reinterpret_cast<Elf_Rela*>(rela + i);
		auto type = ELF_R_TYPE(entry.r_info);

		if (type == R_COPY || type == R_IRELATIVE) {
			continue;
		}

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, lookup_policy);
		}

		has_unresolved_symbols |= !process_relocation({this, entry}, resolved_sym);
	}

	if (!SAVED_LIBC_REL_ADDENDS) {
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
				for (int j = 0;; ++j) {
					entry >>= 1;
					if (!entry) {
						break;
					}
					else if (entry & 1) {
						relr_addr[j] += base;
					}
				}
				// each entry describes at max 63 or 31 locations
				relr_addr += 8 * sizeof(Elf_Relr) - 1;
			}
		}
	}

	size_t libc_saved_addend_index = 0;

	for (uintptr_t i = 0; i < rel_size; i += sizeof(Elf_Rel)) {
		auto entry = *reinterpret_cast<Elf_Rel*>(rel + i);
		auto type = ELF_R_TYPE(entry.r_info);

		if (type == R_COPY || type == R_IRELATIVE) {
			continue;
		}

		Relocation reloc {this, entry, 0};

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, lookup_policy);

			if (SAVED_LIBC_REL_ADDENDS) {
				reloc.rela.r_addend = SAVED_LIBC_REL_ADDENDS[libc_saved_addend_index++];
			}
			else {
				reloc = {this, entry};
			}
		}
		else {
			if (SAVED_LIBC_REL_ADDENDS) {
				continue;
			}

			reloc = {this, entry};
		}

		has_unresolved_symbols |= !process_relocation(reloc, resolved_sym);
	}

	if (plt_rel_size) {
		if (plt_rel_type == DT_RELA) {
			for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rela)) {
				auto entry = *reinterpret_cast<Elf_Rela*>(plt_rel + i);
				auto type = ELF_R_TYPE(entry.r_info);
				if (type == R_COPY || type == R_IRELATIVE) {
					continue;
				}

				hz::optional<ObjectSymbol> resolved_sym;
				auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
				if (ELF_R_SYM(entry.r_info)) {
					const char* sym_name = strtab + sym->st_name;
					resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, lookup_policy);
				}

				has_unresolved_symbols |= !process_relocation({this, entry}, resolved_sym);
			}
		}
		else {
			__ensure(plt_rel_type == DT_REL);

			for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rel)) {
				auto entry = *reinterpret_cast<Elf_Rel*>(plt_rel + i);
				auto type = ELF_R_TYPE(entry.r_info);
				if (type == R_COPY || type == R_IRELATIVE) {
					continue;
				}

				Relocation reloc {this, entry, 0};

				hz::optional<ObjectSymbol> resolved_sym;
				auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
				if (ELF_R_SYM(entry.r_info)) {
					const char* sym_name = strtab + sym->st_name;
					resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, lookup_policy);

					if (SAVED_LIBC_REL_ADDENDS) {
						reloc.rela.r_addend = SAVED_LIBC_REL_ADDENDS[libc_saved_addend_index++];
					}
					else {
						reloc = {this, entry};
					}
				}
				else {
					if (SAVED_LIBC_REL_ADDENDS) {
						continue;
					}

					reloc = {this, entry};
				}

				has_unresolved_symbols |= !process_relocation(reloc, resolved_sym);
			}
		}
	}

	if (has_unresolved_symbols) {
		panic("object had unresolved symbols");
	}
}

void SharedObject::late_relocate() {
	uintptr_t rela = 0;
	uintptr_t rel = 0;
	uintptr_t rela_size = 0;
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

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, LookupPolicy::IgnoreLocal);
		}

		process_late_relocation({this, entry}, resolved_sym);
	}

	for (uintptr_t i = 0; i < rel_size; i += sizeof(Elf_Rel)) {
		auto entry = *reinterpret_cast<Elf_Rel*>(rel + i);
		auto type = ELF_R_TYPE(entry.r_info);

		if (type != R_COPY && type != R_IRELATIVE) {
			continue;
		}

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, LookupPolicy::IgnoreLocal);
		}

		process_late_relocation({this, entry}, resolved_sym);
	}

	if (plt_rel_size) {
		if (plt_rel_type == DT_RELA) {
			for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rela)) {
				auto entry = *reinterpret_cast<Elf_Rela*>(plt_rel + i);
				auto type = ELF_R_TYPE(entry.r_info);
				if (type != R_COPY && type != R_IRELATIVE) {
					continue;
				}

				hz::optional<ObjectSymbol> resolved_sym;
				auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
				if (ELF_R_SYM(entry.r_info)) {
					const char* sym_name = strtab + sym->st_name;
					resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, LookupPolicy::IgnoreLocal);
				}

				process_late_relocation({this, entry}, resolved_sym);
			}
		}
		else {
			__ensure(plt_rel_type == DT_REL);

			for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rel)) {
				auto entry = *reinterpret_cast<Elf_Rel*>(plt_rel + i);
				auto type = ELF_R_TYPE(entry.r_info);
				if (type != R_COPY && type != R_IRELATIVE) {
					continue;
				}

				hz::optional<ObjectSymbol> resolved_sym;
				auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
				if (ELF_R_SYM(entry.r_info)) {
					const char* sym_name = strtab + sym->st_name;
					resolved_sym = OBJECT_STORAGE->lookup(this, sym_name, LookupPolicy::IgnoreLocal);
				}

				process_late_relocation({this, entry}, resolved_sym);
			}
		}
	}
}

void SharedObject::relocate_libc(intptr_t* saved_rel_addends) {
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

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			if (auto elf_sym = lookup(sym_name)) {
				resolved_sym = ObjectSymbol {this, elf_sym};
			}
		}

		has_unresolved_symbols |= !process_relocation({this, entry}, resolved_sym);
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
			for (int j = 0;; ++j) {
				entry >>= 1;
				if (!entry) {
					break;
				}
				else if (entry & 1) {
					relr_addr[j] += base;
				}
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

		hz::optional<ObjectSymbol> resolved_sym;
		auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
		if (ELF_R_SYM(entry.r_info)) {
			const char* sym_name = strtab + sym->st_name;
			if (auto elf_sym = lookup(sym_name)) {
				resolved_sym = ObjectSymbol {this, elf_sym};
			}

			__ensure(SAVED_LIBC_REL_ADDEND_COUNT < MAX_SAVED_LIBC_REL_ADDENDS);
			saved_rel_addends[SAVED_LIBC_REL_ADDEND_COUNT++] = *reinterpret_cast<intptr_t*>(base + entry.r_offset);
		}

		has_unresolved_symbols |= !process_relocation({this, entry}, resolved_sym);
	}

	if (plt_rel_size) {
		if (plt_rel_type == DT_RELA) {
			for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rela)) {
				auto entry = *reinterpret_cast<Elf_Rela*>(plt_rel + i);
				auto type = ELF_R_TYPE(entry.r_info);
				if (type == R_COPY || type == R_IRELATIVE) {
					continue;
				}

				hz::optional<ObjectSymbol> resolved_sym;
				auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
				if (ELF_R_SYM(entry.r_info)) {
					const char* sym_name = strtab + sym->st_name;
					if (auto elf_sym = lookup(sym_name)) {
						resolved_sym = ObjectSymbol {this, elf_sym};
					}
				}

				has_unresolved_symbols |= !process_relocation({this, entry}, resolved_sym);
			}
		}
		else {
			__ensure(plt_rel_type == DT_REL);

			for (uintptr_t i = 0; i < plt_rel_size; i += sizeof(Elf_Rel)) {
				auto entry = *reinterpret_cast<Elf_Rel*>(plt_rel + i);
				auto type = ELF_R_TYPE(entry.r_info);
				if (type == R_COPY || type == R_IRELATIVE) {
					continue;
				}

				hz::optional<ObjectSymbol> resolved_sym;
				auto* sym = &symtab[ELF_R_SYM(entry.r_info)];
				if (ELF_R_SYM(entry.r_info)) {
					const char* sym_name = strtab + sym->st_name;
					if (auto elf_sym = lookup(sym_name)) {
						resolved_sym = ObjectSymbol {this, elf_sym};
					}

					__ensure(SAVED_LIBC_REL_ADDEND_COUNT < MAX_SAVED_LIBC_REL_ADDENDS);
					saved_rel_addends[SAVED_LIBC_REL_ADDEND_COUNT++] = *reinterpret_cast<intptr_t*>(base + entry.r_offset);
				}

				has_unresolved_symbols |= !process_relocation({this, entry}, resolved_sym);
			}
		}
	}

	if (has_unresolved_symbols) {
		panic("object had unresolved symbols");
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
