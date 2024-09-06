.section .text
.globl _start
.type _start, @function

.cfi_startproc
_start:
	lea main(%rip), %rdi
	mov (%rsp), %rsi
	lea 8(%rsp), %rdx
	call __libc_start_main
.cfi_endproc

.section .note.GNU-stack
