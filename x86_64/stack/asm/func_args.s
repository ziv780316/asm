	.file	"func_args.c"
	.text
	.globl	func
	.type	func, @function
func:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movl	%ecx, -32(%rbp)
	movl	%r8d, -36(%rbp)
	movl	%r9d, -40(%rbp)
	movss	%xmm0, -44(%rbp)
	movss	%xmm1, -48(%rbp)
	movsd	%xmm2, -56(%rbp)
	movsd	%xmm3, -64(%rbp)
	movsd	%xmm4, -72(%rbp)
	movsd	%xmm5, -80(%rbp)
	movsd	%xmm6, -88(%rbp)
	movsd	%xmm7, -96(%rbp)
	movl	$1, -4(%rbp)
	movb	$49, -6(%rbp)
	movb	$50, -5(%rbp)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	func, .-func
	.section	.rodata
.LC0:
	.string	"s2"
.LC14:
	.string	"s1"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$192, %rsp
	movl	%edi, -180(%rbp)
	movq	%rsi, -192(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	.LC0(%rip), %rax
	movq	%rax, -128(%rbp)
	movl	$1, -172(%rbp)
	movl	$2, -168(%rbp)
	movl	$3, -164(%rbp)
	movl	$4, -160(%rbp)
	movl	$5, -156(%rbp)
	movl	$6, -152(%rbp)
	movl	$7, -148(%rbp)
	movl	$8, -144(%rbp)
	movabsq	$45035996273704959, %rax
	movq	%rax, -120(%rbp)
	movabsq	$76561193665298431, %rax
	movq	%rax, -112(%rbp)
	movss	.LC1(%rip), %xmm0
	movss	%xmm0, -140(%rbp)
	movss	.LC2(%rip), %xmm0
	movss	%xmm0, -136(%rbp)
	movsd	.LC3(%rip), %xmm0
	movsd	%xmm0, -104(%rbp)
	movsd	.LC4(%rip), %xmm0
	movsd	%xmm0, -96(%rbp)
	movsd	.LC5(%rip), %xmm0
	movsd	%xmm0, -88(%rbp)
	movsd	.LC6(%rip), %xmm0
	movsd	%xmm0, -80(%rbp)
	movsd	.LC7(%rip), %xmm0
	movsd	%xmm0, -72(%rbp)
	movsd	.LC8(%rip), %xmm0
	movsd	%xmm0, -64(%rbp)
	movsd	.LC9(%rip), %xmm0
	movsd	%xmm0, -56(%rbp)
	movsd	.LC10(%rip), %xmm0
	movsd	%xmm0, -48(%rbp)
	movsd	.LC11(%rip), %xmm0
	movsd	%xmm0, -40(%rbp)
	movsd	.LC12(%rip), %xmm0
	movsd	%xmm0, -32(%rbp)
	movss	.LC13(%rip), %xmm0
	movss	%xmm0, -132(%rbp)
	leaq	.LC14(%rip), %rax
	movq	%rax, -24(%rbp)
	leaq	-128(%rbp), %rax
	movq	%rax, -16(%rbp)
	movsd	-64(%rbp), %xmm7
	movsd	-72(%rbp), %xmm6
	movsd	-80(%rbp), %xmm5
	movsd	-88(%rbp), %xmm4
	movsd	-96(%rbp), %xmm3
	movsd	-104(%rbp), %xmm2
	movss	-136(%rbp), %xmm1
	movl	-140(%rbp), %edi
	movl	-152(%rbp), %r9d
	movl	-156(%rbp), %r10d
	movl	-160(%rbp), %ecx
	movl	-164(%rbp), %edx
	movl	-168(%rbp), %esi
	movl	-172(%rbp), %eax
	subq	$8, %rsp
	pushq	-16(%rbp)
	pushq	-24(%rbp)
	movss	-132(%rbp), %xmm0
	leaq	-8(%rsp), %rsp
	movss	%xmm0, (%rsp)
	pushq	-32(%rbp)
	pushq	-40(%rbp)
	pushq	-48(%rbp)
	pushq	-56(%rbp)
	pushq	-112(%rbp)
	pushq	-120(%rbp)
	movl	-144(%rbp), %r8d
	pushq	%r8
	movl	-148(%rbp), %r8d
	pushq	%r8
	movl	%edi, -184(%rbp)
	movss	-184(%rbp), %xmm0
	movl	%r10d, %r8d
	movl	%eax, %edi
	call	func@PLT
	addq	$96, %rsp
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	xorq	%fs:40, %rdx
	je	.L5
	call	__stack_chk_fail@PLT
.L5:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC1:
	.long	1065353216
	.align 4
.LC2:
	.long	1073741824
	.align 8
.LC3:
	.long	3435973837
	.long	1074318540
	.align 8
.LC4:
	.long	1717986918
	.long	1074816614
	.align 8
.LC5:
	.long	1717986918
	.long	1075078758
	.align 8
.LC6:
	.long	1717986918
	.long	1075340902
	.align 8
.LC7:
	.long	1717986918
	.long	1075603046
	.align 8
.LC8:
	.long	858993459
	.long	1075852083
	.align 8
.LC9:
	.long	858993459
	.long	1075983155
	.align 8
.LC10:
	.long	858993459
	.long	1076114227
	.align 8
.LC11:
	.long	858993459
	.long	1076245299
	.align 8
.LC12:
	.long	858993459
	.long	1076376371
	.align 4
.LC13:
	.long	1095866778
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.11) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
