#include "ucontext.h"
#include "utils.hpp"
#include <stdarg.h>

#define STR_HELPER(value) #value
#define STR(value) STR_HELPER(value)
#define REG_OFFSET_VALUE(reg) MCONTEXT_OFFSET + (reg) * REG_SIZE // NOLINT(*-macro-parentheses)
#define REG_OFFSET(reg) STR(REG_OFFSET_VALUE(reg))

#ifdef __x86_64__

static_assert(offsetof(ucontext_t, uc_mcontext) == 40);

#define MCONTEXT_OFFSET 40
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

#define MCONTEXT_OFFSET 20
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
	"movl $0, " REG_OFFSET(REG_EAX) "(%eax);"
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

#elif defined(__aarch64__)

static_assert(offsetof(ucontext_t, uc_mcontext) == 176);
static_assert(offsetof(mcontext_t, sp) == 256);
static_assert(offsetof(mcontext_t, pc) == 264);
static_assert(offsetof(mcontext_t, pstate) == 272);
static_assert(offsetof(mcontext_t, __reserved) == 288);

#define MCONTEXT_OFFSET 176
#define REG_SIZE 8

#define SP_OFFSET "#(176 + 256)"
#define PC_OFFSET "#(176 + 264)"
#define PSTATE_OFFSET "#(176 + 272)"
#define FP_OFFSET "#(176 + 288)"

asm(R"(
.pushsection .text
.globl getcontext
.type getcontext, @function
// int getcontext(ucontext_t* ctx)
getcontext:)"
	"str xzr, [x0, " REG_OFFSET(0) "];"
	"stp x2, x3, [x0, " REG_OFFSET(2) "];"
	"str x30, [x0, " PC_OFFSET "];"
	"mov x2, sp;"
	"str x2, [x0, " SP_OFFSET "];"
	"str xzr, [x0, " PSTATE_OFFSET "];"
	"add x2, x0, " FP_OFFSET ";"
	"stp q8, q9, [x2, #144];"
	"stp q10, q11, [x2, #176];"
	"stp q12, q13, [x2, #208];"
	"stp q14, q15, [x2, #240];"
	"mov x2, x0;"
	"mov x0, #0;"
	"stp x0, x1, [x2, " REG_OFFSET(0) "];"
	"stp x4, x5, [x2, " REG_OFFSET(4) "];"
	"stp x6, x7, [x2, " REG_OFFSET(6) "];"
	"stp x8, x9, [x2, " REG_OFFSET(8) "];"
	"stp x10, x11, [x2, " REG_OFFSET(10) "];"
	"stp x12, x13, [x2, " REG_OFFSET(12) "];"
	"stp x14, x15, [x2, " REG_OFFSET(14) "];"
	"stp x16, x17, [x2, " REG_OFFSET(16) "];"
	"stp x18, x19, [x2, " REG_OFFSET(18) "];"
	"stp x20, x21, [x2, " REG_OFFSET(20) "];"
	"stp x22, x23, [x2, " REG_OFFSET(22) "];"
	"stp x24, x25, [x2, " REG_OFFSET(24) "];"
	"stp x26, x27, [x2, " REG_OFFSET(26) "];"
	"stp x28, x29, [x2, " REG_OFFSET(28) "];"
	"str x30, [x2, " REG_OFFSET(30) "];"
	"ret"
	);

asm(R"(
.pushsection .text
.globl setcontext
.type setcontext, @function
// int setcontext(const ucontext_t* ctx)
setcontext:
)"
	"ldp x18, x19, [x0, " REG_OFFSET(18) "];"
	"ldp x20, x21, [x0, " REG_OFFSET(20) "];"
	"ldp x22, x23, [x0, " REG_OFFSET(22) "];"
	"ldp x24, x25, [x0, " REG_OFFSET(24) "];"
	"ldp x26, x27, [x0, " REG_OFFSET(26) "];"
	"ldp x28, x29, [x0, " REG_OFFSET(28) "];"
	"ldr x30, [x0, " REG_OFFSET(30) "];"
	"ldr x2, [x0, " SP_OFFSET "];"
	"mov sp, x2;"
	"add x2, x0, " FP_OFFSET ";"
	"ldp q8, q9, [x2, #144];"
	"ldp q10, q11, [x2, #176];"
	"ldp q12, q13, [x2, #208];"
	"ldp q14, q15, [x2, #240];"
	"ldr x16, [x0, " PC_OFFSET "];"
	"ldp x2, x3, [x0, " REG_OFFSET(2) "];"
	"ldp x4, x5, [x0, " REG_OFFSET(4) "];"
	"ldp x6, x7, [x0, " REG_OFFSET(6) "];"
	"ldp x0, x1, [x0, " REG_OFFSET(0) "];"
	"br x16"
	);

void trampoline() {
	ucontext_t* link;
	asm volatile("mov %0, x19" : "=r"(link));
	setcontext(link);
}

EXPORT void makecontext(ucontext_t* ctx, void (*func)(), int argc, ...) {
	auto* sp = reinterpret_cast<greg_t*>(
		reinterpret_cast<uintptr_t>(ctx->uc_stack.ss_sp) +
		ctx->uc_stack.ss_size);

	sp -= argc < 8 ? 0 : argc - 8;
	sp = reinterpret_cast<greg_t*>(
		reinterpret_cast<uintptr_t>(sp) & ~(16 - 1)
	);

	ctx->uc_mcontext.sp = reinterpret_cast<greg_t>(sp);
	ctx->uc_mcontext.pc = reinterpret_cast<greg_t>(func);
	ctx->uc_mcontext.regs[19] = reinterpret_cast<greg_t>(ctx->uc_link);
	ctx->uc_mcontext.regs[30] = reinterpret_cast<greg_t>(trampoline);

	va_list ap;
	va_start(ap, argc);
	for (int i = 0; i < argc; ++i) {
		if (i < 8) {
			ctx->uc_mcontext.regs[i] = va_arg(ap, greg_t);
		}
		else {
			sp[i - 8] = va_arg(ap, greg_t);
		}
	}

	va_end(ap);
}

#else
#error missing architecture specific code
#endif
