.section .init
	add $8, %esp
	pop %ebp
	ret

.section .fini
	add $8, %esp
	pop %ebp
	ret

.section .note.GNU-stack
