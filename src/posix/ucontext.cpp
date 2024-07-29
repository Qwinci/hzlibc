#include "ucontext.h"
#include "utils.hpp"
#include <stdarg.h>

#define STR_HELPER(value) #value
#define STR(value) STR_HELPER(value)
#define REG_OFFSET_VALUE(reg) 40 + (reg) * REG_SIZE // NOLINT(*-macro-parentheses)
#define REG_OFFSET(reg) STR(REG_OFFSET_VALUE(reg))

#ifdef __x86_64__

static_assert(offsetof(ucontext_t, uc_mcontext) == 40);

#define REG_SIZE 8

asm(R"(
.pushsection .text
.globl getcontext
.type getcontext, @function
// int getcontext(ucontext_t* ctx)
getcontext:)"
	"mov %r8, " REG_OFFSET(REG_R8) "(%rdi);"
	"mov %r9, " REG_OFFSET(REG_R9) "(%rdi);"
	"mov %r10, " REG_OFFSET(REG_R10) "(%rdi);"
	"mov %r11, " REG_OFFSET(REG_R11) "(%rdi);"
	"mov %r12, " REG_OFFSET(REG_R12) "(%rdi);"
	"mov %r13, " REG_OFFSET(REG_R13) "(%rdi);"
	"mov %r14, " REG_OFFSET(REG_R14) "(%rdi);"
	"mov %r15, " REG_OFFSET(REG_R15) "(%rdi);"
	"mov %rsi, " REG_OFFSET(REG_RSI) "(%rdi);"
	"mov %rbp, " REG_OFFSET(REG_RBP) "(%rdi);"
	"mov %rbx, " REG_OFFSET(REG_RBX) "(%rdi);"
	"mov %rdx, " REG_OFFSET(REG_RDX) "(%rdi);"
	"mov %rax, " REG_OFFSET(REG_RAX) "(%rdi);"
	"mov %rcx, " REG_OFFSET(REG_RCX) "(%rdi);"
	"lea 8(%rsp), %rcx;"
	"mov %rcx, " REG_OFFSET(REG_RSP) "(%rdi);"
	"mov (%rsp), %rcx;"
	"mov %rcx, " REG_OFFSET(REG_RIP) "(%rdi);"
	"xor %eax, %eax;"
	"ret"
	);

asm(R"(
.pushsection .text
.globl setcontext
.type setcontext, @function
// int setcontext(const ucontext_t* ctx)
setcontext:
)"
	"mov " REG_OFFSET(REG_R8) "(%rdi), %r8;"
	"mov " REG_OFFSET(REG_R9) "(%rdi), %r9;"
	"mov " REG_OFFSET(REG_R10) "(%rdi), %r10;"
	"mov " REG_OFFSET(REG_R11) "(%rdi), %r11;"
	"mov " REG_OFFSET(REG_R12) "(%rdi), %r12;"
	"mov " REG_OFFSET(REG_R13) "(%rdi), %r13;"
	"mov " REG_OFFSET(REG_R14) "(%rdi), %r14;"
	"mov " REG_OFFSET(REG_R15) "(%rdi), %r15;"
	"mov " REG_OFFSET(REG_RSI) "(%rdi), %rsi;"
	"mov " REG_OFFSET(REG_RBP) "(%rdi), %rbp;"
	"mov " REG_OFFSET(REG_RBX) "(%rdi), %rbx;"
	"mov " REG_OFFSET(REG_RDX) "(%rdi), %rdx;"
	"mov " REG_OFFSET(REG_RAX) "(%rdi), %rax;"
	"mov " REG_OFFSET(REG_RCX) "(%rdi), %rcx;"
	"mov " REG_OFFSET(REG_RSP) "(%rdi), %rsp;"
	"push " REG_OFFSET(REG_RIP) "(%rdi);"
	"mov " REG_OFFSET(REG_RDI) "(%rdi), %rdi;"
	"xor %eax, %eax;"
	"ret"
);

void trampoline() {
	ucontext_t* link;
	asm volatile("mov (%%rbx), %0" : "=r"(link));
	setcontext(link);
}

namespace {
	constexpr int REGS[] {REG_RDI, REG_RSI, REG_RDX, REG_RCX, REG_R8, REG_R9};
	constexpr int REG_COUNT = static_cast<int>(sizeof(REGS) / sizeof(*REGS));
}

EXPORT void makecontext(ucontext_t* ctx, void (*func)(), int argc, ...) {
	auto* sp = reinterpret_cast<greg_t*>(
		reinterpret_cast<uintptr_t>(ctx->uc_stack.ss_sp) +
		ctx->uc_stack.ss_size);

	unsigned int link = (argc > 6 ? (argc - 6) : 0) + 1;
	sp -= link;
	sp = reinterpret_cast<greg_t*>(
		(reinterpret_cast<uintptr_t>(sp) & ~(16 - 1)) - 8
		);

	*sp = reinterpret_cast<greg_t>(trampoline);
	sp[link] = reinterpret_cast<greg_t>(ctx->uc_link);

	ctx->uc_mcontext.gregs[REG_RIP] = reinterpret_cast<greg_t>(func);
	ctx->uc_mcontext.gregs[REG_RBX] = reinterpret_cast<greg_t>(&sp[link]);
	ctx->uc_mcontext.gregs[REG_RSP] = reinterpret_cast<greg_t>(sp);

	va_list ap;
	va_start(ap, argc);
	for (int i = 0; i < argc; ++i) {
		if (i < REG_COUNT) {
			ctx->uc_mcontext.gregs[REGS[i]] = va_arg(ap, greg_t);
		}
		else {
			sp[1 + i - REG_COUNT] = va_arg(ap, greg_t);
		}
	}

	va_end(ap);
}

#elif defined(__i386__)

static_assert(offsetof(ucontext_t, uc_mcontext) == 20);

#define REG_SIZE 4

asm(R"(
.pushsection .text
.globl getcontext
.type getcontext, @function
// int getcontext(ucontext_t* ctx)
getcontext:
	mov 4(%esp), %eax
)"
	"mov %edi, " REG_OFFSET(REG_EDI) "(%eax);"
	"mov %esi, " REG_OFFSET(REG_ESI) "(%eax);"
	"mov %ebp, " REG_OFFSET(REG_EBP) "(%eax);"
	"mov %ebx, " REG_OFFSET(REG_EBX) "(%eax);"
	"mov %edx, " REG_OFFSET(REG_EDX) "(%eax);"
	"mov %ecx, " REG_OFFSET(REG_ECX) "(%eax);"
	"mov $0, " REG_OFFSET(REG_EAX) "(%eax);"
	"lea 4(%esp), %ecx;"
	"mov %ecx, " REG_OFFSET(REG_ESP) "(%eax);"
	"mov (%esp), %ecx;"
	"mov %ecx, " REG_OFFSET(REG_EIP) "(%eax);"
	"xor %ecx, %ecx;"
	"mov %fs, %cx;"
	"mov %ecx, " REG_OFFSET(REG_FS) "(%eax);"
	"xor %eax, %eax;"
	"ret"
	);

asm(R"(
.pushsection .text
.globl setcontext
.type setcontext, @function
// int setcontext(const ucontext_t* ctx)
setcontext:
	mov 4(%esp), %eax
)"
	"mov " REG_OFFSET(REG_FS) "(%eax), %ecx;"
	"mov %cx, %fs;"
	"mov " REG_OFFSET(REG_EIP) "(%eax), %ecx;"
	"mov " REG_OFFSET(REG_ESP) "(%eax), %esp;"
	"push %ecx;"
	"mov " REG_OFFSET(REG_EDI) "(%eax), %edi;"
	"mov " REG_OFFSET(REG_ESI) "(%eax), %esi;"
	"mov " REG_OFFSET(REG_EBP) "(%eax), %ebp;"
	"mov " REG_OFFSET(REG_EBX) "(%eax), %ebx;"
	"mov " REG_OFFSET(REG_EDX) "(%eax), %edx;"
	"mov " REG_OFFSET(REG_ECX) "(%eax), %ecx;"
	"mov " REG_OFFSET(REG_EAX) "(%eax), %eax;"
	"ret"
);

void trampoline() {
	ucontext_t* link;
	asm volatile("mov (%%esp, %%ebx, 4), %0" : "=r"(link));
	setcontext(link);
}

EXPORT void makecontext(ucontext_t* ctx, void (*func)(), int argc, ...) {
	auto* sp = reinterpret_cast<greg_t*>(
		reinterpret_cast<uintptr_t>(ctx->uc_stack.ss_sp) +
		ctx->uc_stack.ss_size);

	unsigned int link = (argc > 6 ? (argc - 6) : 0) + 1;
	sp -= link;
	sp = reinterpret_cast<greg_t*>(
		(reinterpret_cast<uintptr_t>(sp) & ~(16 - 1)) - 8
	);

	ctx->uc_mcontext.gregs[REG_EIP] = reinterpret_cast<greg_t>(func);
	ctx->uc_mcontext.gregs[REG_EBX] = argc;
	ctx->uc_mcontext.gregs[REG_ESP] = reinterpret_cast<greg_t>(sp);

	*sp++ = reinterpret_cast<greg_t>(trampoline);

	va_list ap;
	va_start(ap, argc);
	for (int i = 0; i < argc; ++i) {
		*sp++ = va_arg(ap, greg_t);
	}

	va_end(ap);
	*sp = reinterpret_cast<greg_t>(ctx->uc_link);
}

#else
#error missing architecture specific code
#endif
