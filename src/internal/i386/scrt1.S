.section .text
.globl _start
.type _start, @function

.cfi_startproc
_start:
	call 1f
1:
	pop %eax
	add $_GLOBAL_OFFSET_TABLE_ + (. - 1b), %eax

	// argv
	lea 4(%esp), %ecx
	push %ecx

	// argc
	mov (%esp), %edx
	push %edx

	push main@GOT(%eax)
	call __libc_start_main@plt
.cfi_endproc

.section .note.GNU-stack
