	.global _start
	.section __TEXT,__text
_start:
	mov $1, %rax
	mov $1, %rdi
	lea msg(%rip), %rsi
	mov $13, %rdx
	syscall

	mov $60, %rax
	mov $0, %rdi
	syscall

msg:
	.asciz "Hello World!\n"