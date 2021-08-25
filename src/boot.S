#include "mem.h"
#include "reg.h"

// The core to boot on
#define MAIN_CORE 0
#define CORE_COUNT 2

.section ".text.boot"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0x3
    cmp x0, #0
    bne hang

    ldr x2, =__EL0_stack_core0
    ldr x3, =__EL1_stack_core0
    msr sp_el0, x2
    msr sp_el1, x3

    //sub x0, x0, #MAIN_CORE
    //cbnz x0, hang

    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0  

    ldr x0, =HCR_VALUE
    msr hcr_el2, x0

    ldr x0, =SCR_VALUE
    msr scr_el3, x0

    ldr x0, =SPSR_EL3_TO_EL1h
    msr spsr_el3, x0

    adr x0, el1_entry
    msr elr_el3, x0

    eret

el1_entry:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero

    //mov sp, #LOW_MEMORY
    bl kernel_main
hang:
    wfe
    b hang