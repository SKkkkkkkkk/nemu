/* Entry point for bare metal programs */
.section .text.entrypoint
.global entrypoint
.type entrypoint, @function

entrypoint:
/* initialize stack pointer */
	la sp, _stack_end

/* set vector table address */
	# la a0, __vector_start
	# ori a0, a0, 1 /*vector mode = vectored */
	# csrw mtvec, a0

/* clear the bss segment */
	la a0, _bss_start
	la a2, _bss_end
	sub a2, a2, a0
	li a1, 0
	call memset

/* call main */
//	lw a0, 0(sp)                    /* a0 = argc */
//	addi a1, sp, __SIZEOF_POINTER__ /* a1 = argv */
//	li a2, 0                        /* a2 = envp = NULL */
// Initialize these variables to 0. Cannot use argc or argv
// since the stack is not initialized
	li a0, 0
	li a1, 0
	li a2, 0

	call main
	ebreak
	// j .
	//tail exit

.size  entrypoint, .-entrypoint