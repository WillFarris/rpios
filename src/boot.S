#include "reg.h"
#include "gpio.h"

#define MASTER_CORE 0

.section ".text.boot"

.globl _start
_start:
    mrs x0, mpidr_el1
    and x0, x0, #0x3

    cmp x0, #0
    beq core0_stack
    
    cmp x0, #1
    beq core1_stack
    
    cmp x0, #2
    beq hang
    
    cmp x0, #3
    beq hang

hang:
    wfe
    b hang

core0_stack:
    ldr x2, =__EL0_stack_core0
    ldr x3, =__EL1_stack_core0
    ldr x4, =__EL2_stack_core0
    b set_stack
core1_stack:
    ldr x2, =__EL0_stack_core1
    ldr x3, =__EL1_stack_core1
    ldr x4, =__EL2_stack_core1
    b set_stack
core2_stack:
    ldr x2, =__EL0_stack_core2
    ldr x3, =__EL1_stack_core2
    ldr x4, =__EL2_stack_core2
    b set_stack
core3_stack:
    ldr x2, =__EL0_stack_core3
    ldr x3, =__EL1_stack_core3
    ldr x4, =__EL2_stack_core3
    b set_stack

set_stack:
    msr sp_el0, x2
    msr sp_el1, x3
    //msr sp_el2, x4

    cmp x0, #MASTER_CORE
    bne slave_core_sleep

    ldr x0, =SCTLR_VALUE_MMU_DISABLED
    msr sctlr_el1, x0  

    ldr x0, =HCR_VALUE
    msr hcr_el2, x0

    ldr x0, =SCR_VALUE
    msr scr_el3, x0

    ldr x0, =SPSR_EL3_TO_EL1h
    msr spsr_el3, x0

    adr x0, el1_entry_master_core
    msr elr_el3, x0

    eret

el1_entry_master_core:
    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero
    
    bl irq_init_vectors
    
    bl kernel_main

.balign 4
slave_core_sleep:
    wfe
    mrs x0, mpidr_el1
    and x0, x0, #3
    ldr x1, =CPU_CORE0_MAILBOX_READ
    mov x3, #16
    mul x2, x0, x3
    add x1, x1, x2

    ldr x1, [x1]
    
    dmb sy // data memory buffer
    blr x1 //branch and link to register
    dmb sy
    b slave_core_sleep

.globl core_execute
core_execute:
    dmb sy
    mov x3, #16
    mul x0, x0, x3
    ldr x2, =CPU_CORE0_MAILBOX_WRITE
    add x2, x2, x0
    str w1, [x2]
    sev
    dmb sy
    ret
