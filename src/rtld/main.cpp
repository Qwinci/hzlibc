#include "sys.hpp"
#include "elf.hpp"
#include "shared_object.hpp"
#include "tcb.hpp"
#include "object_storage.hpp"
#include "rtld.hpp"
#include "sys/mman.h"
#include "opts.hpp"
#include <hz/manually_init.hpp>
#include <stdint.h>

static_assert(sizeof(uintptr_t) == sizeof(size_t));
static_assert(alignof(uintptr_t) == alignof(size_t));

#ifdef __x86_64__

asm(R"(
.pushsection .text
.globl _start
.hidden _start
.hidden start
_start:
	mov %rsp, %rdi
	call start
	jmp *%rax

.globl get_dynamic
.globl get_ehdr

.weak _DYNAMIC
.hidden _DYNAMIC
get_dynamic:
	lea _DYNAMIC(%rip), %rax
	ret

.weak __ehdr_start
.hidden __ehdr_start
get_ehdr:
	lea __ehdr_start(%rip), %rax
	ret

.popsection
)");

#elif defined(__i386__)

asm(R"(
.pushsection .text
.globl _start
.hidden _start
.hidden start
_start:
	sub $12, %esp
	lea 12(%esp), %eax
	push %eax
	call start
	add $16, %esp
	jmp *%eax

.globl get_dynamic
.globl get_ehdr

.weak _DYNAMIC
.hidden _DYNAMIC
get_dynamic:
	call 1f
1:
	pop %eax
	add $_GLOBAL_OFFSET_TABLE_ + (. - 1b), %eax
	lea _DYNAMIC@GOTOFF(%eax), %eax
	ret

.weak __ehdr_start
.hidden __ehdr_start
get_ehdr:
	call 1f
1:
	pop %eax
	add $_GLOBAL_OFFSET_TABLE_ + (. - 1b), %eax
	lea __ehdr_start@GOTOFF(%eax), %eax
	ret

.popsection
)");

#elif defined(__aarch64__)

asm(R"(
.pushsection .text
.globl _start
.hidden _start
.hidden start
_start:
	mov x0, sp
	bl start
	br x0

.globl get_dynamic
.globl get_ehdr

.weak _DYNAMIC
.hidden _DYNAMIC
get_dynamic:
	adrp x0, _DYNAMIC
	add x0, x0, :lo12:_DYNAMIC
	ret

.weak __ehdr_start
.hidden __ehdr_start
get_ehdr:
	adrp x0, __ehdr_start
	add x0, x0, :lo12:__ehdr_start
	ret

.popsection
)");

#else

#error missing architecture startup code

#endif

extern "C" [[gnu::visibility("hidden")]] Elf_Dyn* get_dynamic();
extern "C" [[gnu::visibility("hidden")]] Elf_Ehdr* get_ehdr();

namespace {
	hz::manually_init<SharedObject> LIBC_OBJECT;
	hz::manually_init<SharedObject> EXE_OBJECT;

	LinkMap* DEBUG_INTERFACE_END = nullptr;
}

DebugInterface DEBUG_INTERFACE {};
EXPORT DebugInterface* _dl_debug_addr = nullptr;

void add_object_to_debug_list(SharedObject* object) {
	if (DEBUG_INTERFACE_END) {
		object->link_map.prev = DEBUG_INTERFACE_END;
		DEBUG_INTERFACE_END->next = &object->link_map;
	}
	else {
		DEBUG_INTERFACE.head = &object->link_map;
	}
	DEBUG_INTERFACE_END = &object->link_map;
}

void remove_object_from_debug_list(SharedObject* object) {
	if (object->link_map.prev) {
		object->link_map.prev->next = object->link_map.next;
	}
	else {
		DEBUG_INTERFACE.head = object->link_map.next;
	}
	if (object->link_map.next) {
		object->link_map.next->prev = object->link_map.prev;
	}
}

extern "C" void _dl_debug_state() {}

void set_debug_state_to_load() {
	DEBUG_INTERFACE.state = RtState::Add;
	_dl_debug_state();
}

void set_debug_state_to_normal() {
	DEBUG_INTERFACE.state = RtState::Consistent;
	_dl_debug_state();
}

void hzlibc_env_init(char** env);
void hzlibc_posix_env_init();
void init_stack_guard(void* entropy);

extern "C" [[gnu::used]] uintptr_t start(uintptr_t* sp) {
	auto argc = *sp;
	auto* argv = reinterpret_cast<char**>(sp + 1);
	auto* envv = argv + argc + 1;
	size_t envc = 0;
	for (auto* env = envv; *env; ++env) ++envc;
	auto* auxv = reinterpret_cast<Elf_Aux*>(envv + envc + 1);

	uintptr_t libc_base = 0;
	Elf_Phdr* exe_phdr = nullptr;
	uint16_t exe_phent = 0;
	uint16_t exe_phnum = 0;
	uintptr_t exe_entry = 0;
	void* entropy = nullptr;

	for (auto* aux = auxv; aux->a_type != AT_NULL; ++aux) {
		switch (aux->a_type) {
			case AT_PHDR:
				exe_phdr = reinterpret_cast<Elf_Phdr*>(aux->a_un.a_val);
				break;
			case AT_PHENT:
				exe_phent = aux->a_un.a_val;
				break;
			case AT_PHNUM:
				exe_phnum = aux->a_un.a_val;
				break;
			case AT_BASE:
				libc_base = aux->a_un.a_val;
				break;
			case AT_ENTRY:
				exe_entry = aux->a_un.a_val;
				break;
			case AT_RANDOM:
				entropy = reinterpret_cast<void*>(aux->a_un.a_val);
			default:
				break;
		}
	}

	if constexpr (LOG_AUX) {
		println(
			"rtld: got ",
			exe_phnum,
			" phdrs (size ",
			exe_phent,
			") at ",
			Fmt::Hex,
			reinterpret_cast<uintptr_t>(exe_phdr),
			Fmt::Dec);
	}

	auto* libc_ehdr = get_ehdr();
	auto* libc_phdr = reinterpret_cast<Elf_Phdr*>(libc_base + libc_ehdr->e_phoff);

	LIBC_OBJECT.initialize(
		&*EXE_OBJECT,
		hz::string<Allocator>::null(Allocator {}),
		hz::string<Allocator>::null(Allocator {}),
		libc_base,
		get_dynamic(),
		libc_phdr,
		libc_ehdr->e_phentsize,
		libc_ehdr->e_phnum);
	LIBC_OBJECT->symbolic_resolution = false;
	LIBC_OBJECT->rtld_loaded = false;

	size_t exe_tls_size = 0;
	size_t exe_tls_align = 1;
	for (int i = 0; i < exe_phnum; ++i) {
		auto phdr = reinterpret_cast<Elf_Phdr*>(reinterpret_cast<uintptr_t>(exe_phdr) + i * exe_phent);
		if (phdr->p_type == PT_TLS) {
			exe_tls_size = phdr->p_memsz;
			exe_tls_align = phdr->p_align;
			break;
		}
	}

	auto exe_tls_offset = (exe_tls_size + (exe_tls_align - 1)) & ~(exe_tls_align - 1);

	LIBC_OBJECT->tls_offset = (exe_tls_offset + LIBC_OBJECT->tls_size + (LIBC_OBJECT->tls_align - 1))
		& ~(LIBC_OBJECT->tls_align - 1);

	intptr_t libc_addends[MAX_SAVED_LIBC_REL_ADDENDS];
	LIBC_OBJECT->relocate_libc(libc_addends);

	for (int i = 0; i < libc_ehdr->e_phnum; ++i) {
		auto* phdr = reinterpret_cast<Elf_Phdr*>(reinterpret_cast<uintptr_t>(libc_phdr) + i * libc_ehdr->e_phentsize);
		LIBC_OBJECT->phdrs_vec.push_back(*phdr);
	}

	init_stack_guard(entropy);

	hz::string<Allocator> exe_path {Allocator {}};
	for (auto* aux = auxv; aux->a_type != AT_NULL; ++aux) {
		if (aux->a_type == AT_EXECFN) {
			exe_path = reinterpret_cast<const char*>(aux->a_un.a_val);
			break;
		}
	}

	DEBUG_INTERFACE.ld_base = reinterpret_cast<void*>(libc_base);

	if (exe_path.empty()) {
		if (argv[0]) {
			exe_path = argv[0];
		}
	}

	uintptr_t exe_base = 0;
	uintptr_t exe_dynamic_offset = 0;
	uintptr_t exe_interp_offset = 0;
	uintptr_t exe_dynamic_size = 0;

	for (int i = 0; i < exe_phnum; ++i) {
		auto* phdr = reinterpret_cast<Elf_Phdr*>(reinterpret_cast<uintptr_t>(exe_phdr) + i * exe_phent);

		if (phdr->p_type == PT_DYNAMIC) {
			exe_dynamic_offset = phdr->p_vaddr;
			exe_dynamic_size = phdr->p_memsz;
		}
		else if (phdr->p_type == PT_INTERP) {
			exe_interp_offset = phdr->p_vaddr;
		}
		else if (phdr->p_type == PT_PHDR) {
			exe_base = reinterpret_cast<uintptr_t>(exe_phdr) - phdr->p_vaddr;
		}
	}

	hz::string<Allocator> libc_path {Allocator {}};
	libc_path = reinterpret_cast<const char*>(exe_base + exe_interp_offset);
	hz::string<Allocator> libc_name {Allocator {}};
	libc_name = "libc.so";

	LIBC_OBJECT->path = std::move(libc_path);
	LIBC_OBJECT->name = std::move(libc_name);
	LIBC_OBJECT->initial_tls_model = true;
	LIBC_OBJECT->link_map.name = LIBC_OBJECT->path.data();

	auto* exe_dynamic = reinterpret_cast<Elf_Dyn*>(exe_base + exe_dynamic_offset);

	__ensure(sys_mprotect(
		reinterpret_cast<void*>((exe_base + exe_dynamic_offset) & ~0xFFF),
		(((exe_base + exe_dynamic_offset) & 0xFFF) + exe_dynamic_size + 0xFFF) & ~0xFFF,
		PROT_READ | PROT_WRITE) == 0 && "failed to make executable dynamic section writable");

	hz::string<Allocator> exe_name {Allocator {}};
	exe_name = "<executable>";
	EXE_OBJECT.initialize(
		nullptr,
		std::move(exe_name),
		std::move(exe_path),
		exe_base,
		exe_dynamic,
		exe_phdr,
		exe_phent,
		exe_phnum);
	EXE_OBJECT->rtld_loaded = false;
	EXE_OBJECT->tls_offset = exe_tls_offset;
	EXE_OBJECT->executable = true;
	EXE_OBJECT->initial_tls_model = true;

	OBJECT_STORAGE.initialize();
	OBJECT_STORAGE->add_object(&*EXE_OBJECT);
	OBJECT_STORAGE->global_scope.push_back(&*EXE_OBJECT);
	OBJECT_STORAGE->add_object(&*LIBC_OBJECT);
	OBJECT_STORAGE->global_scope.push_back(&*LIBC_OBJECT);
	OBJECT_STORAGE->initial_tls_size = LIBC_OBJECT->tls_offset;
	__ensure(OBJECT_STORAGE->load_dependencies(&*EXE_OBJECT, true, true) == LoadError::Success);

	// relocate libc again to take into account preloads
	SAVED_LIBC_REL_ADDENDS = libc_addends;
	LIBC_OBJECT->relocate();
	SAVED_LIBC_REL_ADDENDS = nullptr;

	LIBC_OBJECT->late_relocate();

	__ensure(sys_mprotect(
		reinterpret_cast<void*>((exe_base + exe_dynamic_offset) & ~0xFFF),
		(((exe_base + exe_dynamic_offset) & 0xFFF) + exe_dynamic_size + 0xFFF) & ~0xFFF,
		PROT_READ) == 0 && "failed to make executable dynamic section readable");

	// allocate the tls
	OBJECT_STORAGE->total_initial_tls_size = OBJECT_STORAGE->initial_tls_size + 1024 * 4;

	void* initial_tcb;
	void* initial_tp;
	__ensure(__dlapi_create_tcb(&initial_tcb, &initial_tp));
#if !ANSI_ONLY
	static_cast<Tcb*>(initial_tcb)->tid = sys_get_thread_id();
#endif
	__ensure(sys_tcb_set(initial_tp) == 0);

	_dl_debug_addr = &DEBUG_INTERFACE;
	DEBUG_INTERFACE.version = 1;
	DEBUG_INTERFACE.brk = &_dl_debug_state;
	DEBUG_INTERFACE.state = RtState::Consistent;
	_dl_debug_state();

	hzlibc_env_init(argv + argc + 1);

#if !ANSI_ONLY
	hzlibc_posix_env_init();
#endif

	if (EXE_OBJECT->preinit_array) {
		for (auto* fn = EXE_OBJECT->preinit_array; fn != EXE_OBJECT->preinit_array_end; ++fn) {
			(*fn)();
		}
	}

	OBJECT_STORAGE->destruct_list.push_back(&*LIBC_OBJECT);
	LIBC_OBJECT->run_init();

	OBJECT_STORAGE->init_objects();

	return exe_entry;
}
