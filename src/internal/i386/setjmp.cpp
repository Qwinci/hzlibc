
#if ANSI_ONLY
extern "C" void __sigsetjmp() {}
#endif

asm(R"(
.pushsection .text
.type __setjmp, @function
__setjmp:
	mov 4(%esp), %eax

	mov %ebx, 0(%eax)
	mov %edi, 4(%eax)
	mov %esi, 8(%eax)
	mov %ebp, 12(%eax)

	lea 4(%esp), %edi
	mov %edi, 16(%eax)

	mov (%esp), %ecx
	mov %ecx, 20(%eax)

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
	mov 4(%esp), %ecx

	mov 0(%ecx), %ebx
	mov 4(%ecx), %edi
	mov 8(%ecx), %esi
	mov 12(%ecx), %ebp

	mov 8(%esp), %eax
	mov 16(%ecx), %esp

	test %eax, %eax
	jnz 1f
	inc %eax
1:
	jmp *20(%ecx)
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
