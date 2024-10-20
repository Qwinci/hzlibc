asm(R"(
.pushsection .text
.globl syscall
.type syscall, @function
syscall:
	mov x8, x0
	mov x0, x1
	mov x1, x2
	mov x2, x3
	mov x3, x4
	mov x4, x5
	mov x5, x6
	mov x6, x7
	svc #0
	cmn x0, #4095
	b.cs 1f
	ret
1:
	neg w0, w0
	adrp x1, :gottprel:__errno
	ldr x1, [x1, :gottprel_lo12:__errno]
	mrs x3, tpidr_el0
	str w0, [x1, x3]
	mov x0, #-1
	ret
.popsection
)");
