asm(R"(
.pushsection .text
.globl syscall
.type syscall, @function
syscall:
	mov %rdi, %rax
	mov %rsi, %rdi
	mov %rdx, %rsi
	mov %rcx, %rdx
	mov %r8, %r10
	mov %r9, %r8
	mov 8(%rsp), %r9
	syscall
	cmp -4096, %rax
	ja 1f
	ret

1:
	neg %rax
	mov __errno@GOTTPOFF(%rip), %rcx
	mov %eax, %fs:0(%rcx)
	mov $-1, %rax
	ret
.popsection
)");
