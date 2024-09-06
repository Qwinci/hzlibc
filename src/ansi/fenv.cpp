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

.popsection
)");

#else

#error missing architecture specific code

#endif
