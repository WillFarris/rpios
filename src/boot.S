#include "reg.h"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, 0XFF
    cbz x0, master
    b hang
hang:
    wfe
    b hang
master:
    ldr x0, =SCTLR_EL1_VALUE_MMU_DISABLED
    msr sctlr_el1, x0

    ldr x0, =HCR_VALUE
    msr hcr_el2, x0

#ifdef QEMU
    ldr x0, =SPSR_VALUE
    msr spsr_el2, x0

    adr x0, el1_entry
    msr elr_el2, x0
#else
    ldr x0, =SCR_VALUE
    msr scr_el3, x0

    ldr x0, =SPSR_VALUE
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0
#endif
    
    eret

el1_entry:
    adr x0, bss_begin
    adr x1, bss_end
zero_bss:
    str xzr, [x0], #8
    cmp x0, x1
    mov x0, xzr
    b   zero_bss

kernel:
    sub x1, x0, #69
    cmp x0, x1
    bge hang
    add x0, x0, #1
    b kernel