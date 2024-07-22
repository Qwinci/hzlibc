#include "ucontext.h"

#define STR_HELPER(value) #value
#define STR(value) STR_HELPER(value)
#define REG_OFFSET_VALUE(reg) 40 + (reg) * REG_SIZE // NOLINT(*-macro-parentheses)
#define REG_OFFSET(reg) STR(REG_OFFSET_VALUE(reg))

#ifdef __x86_64__

static_assert(offsetof(ucontext_t, uc_mcontext) == 40);

#define REG_SIZE 8

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

#elif defined(__i386__)

static_assert(offsetof(ucontext_t, uc_mcontext) == 20);

#define REG_SIZE 4

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

#else
#error missing architecture specific code
#endif
