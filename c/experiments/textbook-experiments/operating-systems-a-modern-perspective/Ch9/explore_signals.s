	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 0	sdk_version 11, 3
	.globl	_main                           ## -- Begin function main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	$0, -4(%rbp)
	movl	$30, %edi
	leaq	_sig_handler(%rip), %rsi
	callq	_signal
	movq	$-1, %rcx
	cmpq	%rax, %rcx
	jne	LBB0_2
## %bb.1:
	leaq	L_.str(%rip), %rdi
	movb	$0, %al
	callq	_printf
LBB0_2:
	movl	$31, %edi
	leaq	_sig_handler(%rip), %rsi
	callq	_signal
	movq	$-1, %rcx
	cmpq	%rax, %rcx
	jne	LBB0_4
## %bb.3:
	leaq	L_.str.1(%rip), %rdi
	movb	$0, %al
	callq	_printf
LBB0_4:
	callq	_getpid
	movl	%eax, -8(%rbp)
	callq	_fork
	xorl	%ecx, %ecx
	movl	%eax, -12(%rbp)
	cmpl	%eax, %ecx
	jne	LBB0_7
## %bb.5:
	movl	-8(%rbp), %edi
	movl	$30, %esi
	callq	_kill
LBB0_6:                                 ## =>This Inner Loop Header: Depth=1
	callq	_pause
	movl	%eax, -20(%rbp)                 ## 4-byte Spill
	jmp	LBB0_6
LBB0_7:
	movl	-12(%rbp), %edi
	movl	$31, %esi
	callq	_kill
	movl	$10, %edi
	movl	%eax, -24(%rbp)                 ## 4-byte Spill
	callq	_sleep
	leaq	L_.str.2(%rip), %rdi
	movl	%eax, -28(%rbp)                 ## 4-byte Spill
	movb	$0, %al
	callq	_printf
	movl	-12(%rbp), %edi
	movl	$15, %esi
	movl	%eax, -32(%rbp)                 ## 4-byte Spill
	callq	_kill
	leaq	-16(%rbp), %rdi
	movl	%eax, -36(%rbp)                 ## 4-byte Spill
	callq	_wait
	leaq	L_.str.3(%rip), %rdi
	movl	%eax, %esi
	movb	$0, %al
	callq	_printf
	movl	-16(%rbp), %esi
	movl	-12(%rbp), %edx
	leaq	L_.str.4(%rip), %rdi
	movl	%eax, -40(%rbp)                 ## 4-byte Spill
	movb	$0, %al
	callq	_printf
	leaq	L_.str.5(%rip), %rdi
	movl	%eax, -44(%rbp)                 ## 4-byte Spill
	movb	$0, %al
	callq	_printf
## %bb.8:
	movl	-4(%rbp), %eax
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90                         ## -- Begin function sig_handler
_sig_handler:                           ## @sig_handler
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, %ecx
	subl	$30, %ecx
	movl	%eax, -8(%rbp)                  ## 4-byte Spill
	je	LBB1_1
	jmp	LBB1_5
LBB1_5:
	movl	-8(%rbp), %eax                  ## 4-byte Reload
	subl	$31, %eax
	je	LBB1_2
	jmp	LBB1_3
LBB1_1:
	leaq	L_.str.6(%rip), %rdi
	movb	$0, %al
	callq	_printf
	jmp	LBB1_4
LBB1_2:
	leaq	L_.str.7(%rip), %rdi
	movb	$0, %al
	callq	_printf
	jmp	LBB1_4
LBB1_3:
	jmp	LBB1_4
LBB1_4:
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"Parent: Unable to create handler SIGUSR1\n"

L_.str.1:                               ## @.str.1
	.asciz	"Parent: Unable to create handler SIGUSR2\n"

L_.str.2:                               ## @.str.2
	.asciz	"Parent: Terminating child...\n"

L_.str.3:                               ## @.str.3
	.asciz	"wait output = %d\n"

L_.str.4:                               ## @.str.4
	.asciz	"%d = status, and %d = child_pid\n"

L_.str.5:                               ## @.str.5
	.asciz	"done\n"

L_.str.6:                               ## @.str.6
	.asciz	"Parent: Received SIGUSR1\n"

L_.str.7:                               ## @.str.7
	.asciz	"Child: Received SIGUSR2\n"

.subsections_via_symbols
