	.text
	.file	"arrayMax.ir"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %MainBegin
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	$10, 20(%rsp)
	movl	$16, 16(%rsp)
	movl	$8, 12(%rsp)
	movl	$10, %edi
	callq	writeln
	movl	16(%rsp), %edi
	callq	writeln
	movl	12(%rsp), %edi
	callq	writeln
	xorl	%eax, %eax
	addq	$24, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object   # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d"
	.size	.L__unnamed_1, 3

	.type	.L__unnamed_2,@object   # @1
.L__unnamed_2:
	.asciz	"\n"
	.size	.L__unnamed_2, 2

	.section	".note.GNU-stack","",@progbits
