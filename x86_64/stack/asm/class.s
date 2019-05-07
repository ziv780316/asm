	.file	"class.cpp"
	.section	.text._ZN4testC2Ev,"axG",@progbits,_ZN4testC5Ev,comdat
	.align 2
	.weak	_ZN4testC2Ev
	.type	_ZN4testC2Ev, @function
_ZN4testC2Ev:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$1, (%rax)
	movq	-8(%rbp), %rax
	movl	$2, 4(%rax)
	movq	_ZN4test8test_cntE@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	leal	1(%rax), %edx
	movq	_ZN4test8test_cntE@GOTPCREL(%rip), %rax
	movl	%edx, (%rax)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	_ZN4testC2Ev, .-_ZN4testC2Ev
	.weak	_ZN4testC1Ev
	.set	_ZN4testC1Ev,_ZN4testC2Ev
	.section	.rodata
.LC0:
	.string	"a=%d b=%d\n"
	.section	.text._ZN4test11show_memberEv,"axG",@progbits,_ZN4test11show_memberEv,comdat
	.align 2
	.weak	_ZN4test11show_memberEv
	.type	_ZN4test11show_memberEv, @function
_ZN4test11show_memberEv:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	4(%rax), %edx
	movq	-8(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	_ZN4test11show_memberEv, .-_ZN4test11show_memberEv
	.section	.rodata
.LC1:
	.string	"test_cnt=%d\n"
	.section	.text._ZN4test11show_staticEv,"axG",@progbits,_ZN4test11show_staticEv,comdat
	.align 2
	.weak	_ZN4test11show_staticEv
	.type	_ZN4test11show_staticEv, @function
_ZN4test11show_staticEv:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	_ZN4test8test_cntE@GOTPCREL(%rip), %rax
	movl	(%rax), %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	_ZN4test11show_staticEv, .-_ZN4test11show_staticEv
	.section	.text._ZN4test3setEii,"axG",@progbits,_ZN4test3setEii,comdat
	.align 2
	.weak	_ZN4test3setEii
	.type	_ZN4test3setEii, @function
_ZN4test3setEii:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movq	-8(%rbp), %rax
	movl	-12(%rbp), %edx
	movl	%edx, (%rax)
	movq	-8(%rbp), %rax
	movl	-16(%rbp), %edx
	movl	%edx, 4(%rax)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	_ZN4test3setEii, .-_ZN4test3setEii
	.globl	_ZN4test8test_cntE
	.bss
	.align 4
	.type	_ZN4test8test_cntE, @object
	.size	_ZN4test8test_cntE, 4
_ZN4test8test_cntE:
	.zero	4
	.section	.rodata
.LC2:
	.string	"sizeof(test)=%lu\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	_ZN4testC1Ev@PLT
	leaq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	_ZN4testC1Ev@PLT
	leaq	-32(%rbp), %rax
	movl	$2, %edx
	movl	$1, %esi
	movq	%rax, %rdi
	call	_ZN4test3setEii@PLT
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	_ZN4test11show_memberEv@PLT
	leaq	-32(%rbp), %rax
	movq	%rax, %rdi
	call	_ZN4test11show_staticEv@PLT
	movl	$8, %esi
	leaq	.LC2(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L7
	call	__stack_chk_fail@PLT
.L7:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.11) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
