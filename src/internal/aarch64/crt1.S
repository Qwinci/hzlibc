.section .text
.globl _start
.type _start, @function

.cfi_startproc
_start:
	adrp x0, main
	add x0, x0, :lo12:main
	ldr x1, [sp]
	add x2, sp, #8
	bl __libc_start_main
	brk #0
.cfi_endproc

.section .note.GNU-stack
