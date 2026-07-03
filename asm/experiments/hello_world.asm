	;; hello_world.asm
        ;;
        ;; Author Zane K.
        ;; Date: 14 06 2024

        global _start

        section .text
	align 8                 ; Align the _start label to an 8-byte boundary
_start:
        mov rax, 0x2000004      ; Use the write syscall = 4
        mov rdi, 1              ; Use the stdout fd
        mov rsi, message        ; Use the message as the buffer
        mov rdx, message_length ; And supply the length
        syscall                 ; Invoke the syscall

        ;; Now gracefully exit
        mov rax, 0x2000001
        mov rdi, 0
        syscall

        section .data
	align 8                 ; Align the message to an 8-byte boundary
message:        db "Hello World!", 0xA
	message_length equ $-message
