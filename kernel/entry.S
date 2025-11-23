        .section .init
        .option norvc

        .type start, @function
        .global start
start:
        .cfi_startproc
        .option push
        .option norelax
        la gp, global_pointer
        .option pop

        /* Reset satp */
        /* csrw satp, zero */

        /* Setup stack */
        la sp, stack_top

        /* Clear the BSS section */
        la t5, bss_start
        la t6, bss_end
bss_clear:
        sw zero, (t5)
        addi t5, t5, 4
        bltu t5, t6, bss_clear

        la t0, kmain
        /* csrw mepc, t0 */

        /* Jump into the kernel */
        tail kmain

        /* Exit the emulator */
        li a0, 0
        li a1, 0
        ecall

        .cfi_endproc
        .end
