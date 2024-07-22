asm(R"(
.pushsection .text
.globl syscall
.type syscall, @function
syscall:
	push %ebx
	push %edi
	push %esi
	push %ebp

	mov 4(%esp), %eax
	mov 8(%esp), %ebx
	mov 12(%esp), %ecx
	mov 16(%esp), %edx
	mov 20(%esp), %esi
	mov 24(%esp), %edi
	mov 28(%esp), %ebp
	int $0x80

	pop %ebp
	pop %esi
	pop %edi
	pop %ebx

	cmp -4096, %eax
	ja 1f
	ret

1:
	neg %eax
	call 2f
2:
	pop %ecx
	add $_GLOBAL_OFFSET_TABLE_, %ecx
	mov __errno@GOTNTPOFF(%ecx), %ecx
	mov %eax, %gs:(%ecx)
	mov $-1, %eax
	ret
.popsection
)");
