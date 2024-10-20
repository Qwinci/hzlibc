#include "fenv.h"
#include "utils.hpp"

#ifdef __x86_64__

asm(R"(
.pushsection .text
// int fesetround(int round)
.globl fesetround
.type fesetround, @function
fesetround:
	push %rax
	mov %edi, %ecx

	fnstcw (%rsp)
	andb $0xF3, 1(%rsp)
	or %ch, 1(%rsp)
	fldcw (%rsp)

	stmxcsr (%rsp)
	shl $3, %ch
	andb $0x9F, 1(%rsp)
	or %ch, 1(%rsp)
	ldmxcsr (%rsp)

	pop %rcx
	xor %eax, %eax
	ret

// int fegetround()
.globl fegetround
.type fegetround, @function
fegetround:
	push %rax
	stmxcsr (%rsp)
	pop %rax
	// RC is in bits 13-14
	shr $3, %eax
	and $0xC00, %eax
	ret

// int feclearexcept(int excepts)
.globl feclearexcept
.type feclearexcept, @function
feclearexcept:
	mov %edi, %ecx
	and $0x3F, %ecx
	fnstsw %ax
	test %eax, %ecx
	jz 1f
	fnclex
1:
	stmxcsr -8(%rsp)
	and $0x3F, %eax
	or %eax, -8(%rsp)
	test %ecx, -8(%rsp)
	jz 1f
	not %ecx
	and %ecx, -8(%rsp)
	ldmxcsr -8(%rsp)
1:
	xor %eax, %eax
	ret

// int fetestexcept(int excepts)
.globl fetestexcept
.type fetestexcept, @function
fetestexcept:
	and $0x3F, %edi
	push %rax
	stmxcsr (%rsp)
	pop %rsi
	fnstsw %ax
	or %esi, %eax
	and %edi, %eax
	ret

// int feraiseexcept(int excepts)
.globl feraiseexcept
.type feraiseexcept, @function
feraiseexcept:
	and $0x3F, %edi
	stmxcsr -8(%rsp)
	or %edi, -8(%rsp)
	ldmxcsr -8(%rsp)
	xor %eax,%eax
	ret

.popsection
)");

#elif defined(__i386__)

asm(R"(
.pushsection .text
// int fesetround(int round)
.globl fesetround
.type fesetround, @function
fesetround:
	push %eax
	mov 4(%esp), %ecx

	fnstcw (%esp)
	andb $0xF3, 1(%esp)
	or %ch, 1(%esp)
	fldcw (%esp)

	// assume sse
	stmxcsr (%esp)
	shl $3, %ch
	andb $0x9F, 1(%esp)
	or %ch, 1(%esp)
	ldmxcsr (%esp)

	pop %ecx
	xor %eax, %eax
	ret

// int fegetround()
.globl fegetround
.type fegetround, @function
fegetround:
	push %eax
	fnstcw (%esp)
	pop %eax
	and $0xC00, %eax
	ret

// int feclearexcept(int excepts)
.globl feclearexcept
.type feclearexcept, @function
feclearexcept:
	mov 4(%esp), %ecx
	and $0x3F, %ecx
	fnstsw %ax
	test %eax, %ecx
	jz 1f
	fnclex
1:
	push %edx
	stmxcsr (%esp)
	pop %edx
	and $0x3F, %eax
	or %eax, %edx
	test %edx, %ecx
	jz 1f
	not %ecx
	and %ecx, %edx
	push %edx
	ldmxcsr (%esp)
	pop %edx
1:
	xor %eax, %eax
	ret

// int fetestexcept(int excepts)
.globl fetestexcept
.type fetestexcept, @function
fetestexcept:
	mov 4(%esp),%ecx
	and $0x3F, %ecx
	fnstsw %ax

	stmxcsr 4(%esp)
	or 4(%esp), %eax
	and %ecx, %eax
	ret

// int feraiseexcept(int excepts)
.globl feraiseexcept
.type feraiseexcept, @function
feraiseexcept:
	mov 4(%esp), %eax
	and $0x3F, %eax
	sub $32, %esp
	fnstenv (%esp)
	or %al, 4(%esp)
	fldenv (%esp)
	add $32, %esp
	xor %eax, %eax
	ret

.popsection
)");

#elif defined(__aarch64__)

asm(R"(
.pushsection .text
// int fesetround(int round)
.globl fesetround
.type fesetround, @function
fesetround:
	mrs x1, fpcr
	bic w1, w1, #0xC00000
	orr w1, w1, w0
	msr fpcr, x1
	mov w0, #0
	ret

// int fegetround()
.globl fegetround
.type fegetround, @function
fegetround:
	mrs x0, fpcr
	and w0, w0, #0xC00000
	ret

// int feclearexcept(int excepts)
.globl feclearexcept
.type feclearexcept, @function
feclearexcept:
	and w0, w0, #0x1F
	mrs x1, fpsr
	bic w1, w1, w0
	msr fpsr, x1
	mov w0, #0
	ret

// int fetestexcept(int excepts)
.globl fetestexcept
.type fetestexcept, @function
fetestexcept:
	and w0, w0, #0x1F
	mrs x1, fpsr
	and w0, w0, w1
	ret

// int feraiseexcept(int excepts)
.globl feraiseexcept
.type feraiseexcept, @function
feraiseexcept:
	and w0, w0, #0x1F
	mrs x1, fpsr
	orr w1, w1, w0
	msr fpsr, x1
	mov w0, #0
	ret

.popsection
)");

#else

#error missing architecture specific code

#endif
