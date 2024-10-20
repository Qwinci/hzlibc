
#if ANSI_ONLY
extern "C" void __sigsetjmp() {}
#endif

asm(R"(
.pushsection .text
.type __setjmp, @function
__setjmp:
	stp x19, x20, [x0, #0]
	stp x21, x22, [x0, #16]
	stp x23, x24, [x0, #32]
	stp x25, x26, [x0, #48]
	stp x27, x28, [x0, #64]
	stp x29, x30, [x0, #80]
	mov x1, sp
	str x1, [x0, #96]

	stp d8, d9, [x0, #112]
	stp d10, d11, [x0, #128]
	stp d12, d13, [x0, #144]
	stp d14, d15, [x0, #160]

	cbnz x2, 1f

	mov x0, xzr
	ret

1:
	b __sigsetjmp

.globl setjmp
.globl _setjmp
.type setjmp, @function
.type _setjmp, @function
// int setjmp(jmp_buf env)
setjmp:
_setjmp:
	mov x2, xzr
	b __setjmp

// void longjmp(jmp_buf env, int val)
.globl longjmp
.globl _longjmp
.type longjmp, @function
.type _longjmp, @function
longjmp:
_longjmp:
	ldp x19, x20, [x0, #0]
	ldp x21, x22, [x0, #16]
	ldp x23, x24, [x0, #32]
	ldp x25, x26, [x0, #48]
	ldp x27, x28, [x0, #64]
	ldp x29, x30, [x0, #80]
	ldr x1, [x0, #96]
	mov sp, x1

	ldp d8, d9, [x0, #112]
	ldp d10, d11, [x0, #128]
	ldp d12, d13, [x0, #144]
	ldp d14, d15, [x0, #160]

	cmp w1, #0
	csinc w0, w1, wzr, ne
	br x30
.popsection
)");

#if !ANSI_ONLY
asm(R"(
.globl sigsetjmp
.type sigsetjmp, @function
sigsetjmp:
	mov x2, #1
	b __setjmp
)");
#endif
