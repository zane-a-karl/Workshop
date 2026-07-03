	;; gcc -o asm -static -nostdlib asm.s

	.global _start
	.section .text
_start:
	// rax is a variable inside of the processor
	// setting rax to one tells it that we are going to write
	mov $1, %rax
	// Setting rdi to one tells it that we are going to write to stdout and not stdin or stderr
	mov $1, %rdi
	// Says we want to write a variable called 'msg' to the screen
	mov $msg, %rsi
	// Save the length of the string in rdx
	mov $13, %rdx
	// Envokes the kernel to do a thing with the data we have previously provided
	syscall

	// Tells the outside world we want to exit our program
	mov $60, %rax
	// Set the error code to 0
	mov $0, %rdi
	// To do() the above two lines
	syscall

msg:
	.asciz "Hello World!\n"