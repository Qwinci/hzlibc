.section .text
.globl _start
.type _start, @function

.cfi_startproc
	// argv
	lea 4(%esp), %ecx
	push %ecx

	// argc
	mov 4(%esp), %edx
	push %edx

	push $main
	call __libc_start_main
.cfi_endproc

.section .note.GNU-stack
