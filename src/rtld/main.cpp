#include "sys.hpp"
#include "elf.hpp"
#include "shared_object.hpp"
#include "tcb.hpp"
#include "object_storage.hpp"
#include "rtld.hpp"
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
	push %rbp
	mov %rsp, %rbp
	call start
	pop %rbp
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
	mov %esp, %edi
	push %ebp
	mov %esp, %ebp
	push %edi
	call start
	add $4, %esp
	pop %ebp
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

#else

#error missing architecture startup code

#endif

extern "C" [[gnu::visibility("hidden")]] Elf_Dyn* get_dynamic();
extern "C" [[gnu::visibility("hidden")]] Elf_Ehdr* get_ehdr();

namespace {
	hz::manually_init<SharedObject> LIBC_OBJECT;
	hz::manually_init<SharedObject> EXE_OBJECT;
}

extern "C" [[gnu::used]] uintptr_t start(uintptr_t* sp) {
	auto argc = *sp;
	auto* argv = reinterpret_cast<char**>(sp + 1);
	auto* envv = argv + argc + 1;
	size_t envc = 0;
	for (auto* env = envv; *env; ++env) ++envc;
	auto* auxv = reinterpret_cast<Elf_Aux*>(envv + envc + 1);

	Elf_Phdr* exe_phdr = nullptr;
	uint16_t exe_phent = 0;
	uint16_t exe_phnum = 0;
	uintptr_t exe_entry = 0;
	uintptr_t libc_base = 0;

	hz::string<Allocator> exe_path {Allocator {}};
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
			case AT_EXECFN:
				exe_path = reinterpret_cast<const char*>(aux->a_un.a_val);
				break;
			default:
				break;
		}
	}

	if (exe_path.empty()) {
		exe_path = argv[0];
	}

	uintptr_t exe_base = 0;
	uintptr_t exe_dynamic_offset = 0;
	uintptr_t exe_interp_offset = 0;

	for (int i = 0; i < exe_phnum; ++i) {
		auto* phdr = reinterpret_cast<Elf_Phdr*>(reinterpret_cast<uintptr_t>(exe_phdr) + i * exe_phent);

		if (phdr->p_type == PT_DYNAMIC) {
			exe_dynamic_offset = phdr->p_vaddr;
		}
		else if (phdr->p_type == PT_INTERP) {
			exe_interp_offset = phdr->p_vaddr;
		}
		else if (phdr->p_type == PT_PHDR) {
			exe_base = reinterpret_cast<uintptr_t>(exe_phdr) - phdr->p_vaddr;
		}
	}

	auto* exe_dynamic = reinterpret_cast<Elf_Dyn*>(exe_base + exe_dynamic_offset);

	auto* libc_ehdr = get_ehdr();
	auto* libc_phdr = reinterpret_cast<Elf_Phdr*>(libc_base + libc_ehdr->e_phoff);
	hz::string<Allocator> libc_path {Allocator {}};
	libc_path = reinterpret_cast<const char*>(exe_base + exe_interp_offset);

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

	hz::string<Allocator> libc_name {Allocator {}};
	libc_name = "libc.so";
	LIBC_OBJECT.initialize(
		&*EXE_OBJECT,
		std::move(libc_name),
		std::move(libc_path),
		libc_base,
		get_dynamic(),
		libc_phdr,
		libc_ehdr->e_phentsize,
		libc_ehdr->e_phnum);
	LIBC_OBJECT->symbolic_resolution = false;
	LIBC_OBJECT->rtld_loaded = false;
	LIBC_OBJECT->tls_offset = (LIBC_OBJECT->tls_size + LIBC_OBJECT->tls_align - 1) & ~(LIBC_OBJECT->tls_align - 1);

	OBJECT_STORAGE.get_unsafe().initialize();
	OBJECT_STORAGE.get_unsafe()->add_object(&*EXE_OBJECT);
	OBJECT_STORAGE.get_unsafe()->global_scope.push_back(&*EXE_OBJECT);
	OBJECT_STORAGE.get_unsafe()->add_object(&*LIBC_OBJECT);
	OBJECT_STORAGE.get_unsafe()->global_scope.push_back(&*LIBC_OBJECT);
	__ensure(OBJECT_STORAGE.get_unsafe()->load_dependencies(&*EXE_OBJECT, true) == LoadError::Success);

	// allocate the tls
	OBJECT_STORAGE.get_unsafe()->total_initial_tls_size = OBJECT_STORAGE.get_unsafe()->initial_tls_size + 1024 * 4;

	void* initial_tcb;
	void* initial_tp;
	__ensure(__dlapi_create_tcb(&initial_tcb, &initial_tp));
	__ensure(sys_tcb_set(initial_tcb) == 0);

	OBJECT_STORAGE.get_unsafe()->init_objects();

	return exe_entry;
}
