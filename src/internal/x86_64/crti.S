.section .init
.globl _init
_init:
	push %rbp
	mov %rsp, %rbp

.section .fini
.globl _fini
_fini:
	push %rbp
	mov %rsp, %rbp

.section .note.GNU-stack
