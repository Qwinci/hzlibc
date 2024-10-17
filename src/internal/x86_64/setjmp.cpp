
#if ANSI_ONLY
extern "C" void __sigsetjmp() {}
#endif

asm(R"(
.pushsection .text
.type __setjmp, @function
__setjmp:
	mov %rbx, 0(%rdi)
	mov %rbp, 8(%rdi)
	mov %r12, 16(%rdi)
	mov %r13, 24(%rdi)
	mov %r14, 32(%rdi)
	mov %r15, 40(%rdi)

	lea 8(%rsp), %rax
	mov %rax, 48(%rdi)

	mov (%rsp), %rcx
	mov %rcx, 56(%rdi)

	test %edx, %edx
	jnz 1f
	xor %eax, %eax
	ret

1:
	jmp __sigsetjmp

.globl setjmp
.globl _setjmp
.type setjmp, @function
.type _setjmp, @function
// int setjmp(jmp_buf env)
setjmp:
_setjmp:
	xor %edx, %edx
	jmp __setjmp

// void longjmp(jmp_buf env, int val)
.globl longjmp
.globl _longjmp
.type longjmp, @function
.type _longjmp, @function
longjmp:
_longjmp:
	mov 0(%rdi), %rbx
	mov 8(%rdi), %rbp
	mov 16(%rdi), %r12
	mov 24(%rdi), %r13
	mov 32(%rdi), %r14
	mov 40(%rdi), %r15
	mov 48(%rdi), %rsp

	mov %esi, %eax
	test %eax, %eax
	jnz 1f
	inc %eax
1:
	jmp *56(%rdi)
.popsection
)");

#if !ANSI_ONLY
asm(R"(
.globl sigsetjmp
.type sigsetjmp, @function
sigsetjmp:
	mov $1, %edx
	jmp __setjmp
)");
#endif
