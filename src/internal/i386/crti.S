.section .init
.globl _init
_init:
	push %ebp
	mov %esp, %ebp
	sub $8, %esp

.section .fini
.globl _fini
_fini:
	push %ebp
	mov %esp, %ebp
	sub $8, %esp

.section .note.GNU-stack
