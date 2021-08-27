#include "reg.h"
#include "gpio.h"

#define MASTER_CORE 0

.section ".text.boot"

.globl _start
_start:
    mrs x5, mpidr_el1
    and x5, x5, #0x3

    cmp x5, #0
    beq core0_stack
    
    cmp x5, #1
    beq core1_stack
    
    cmp x5, #2
    beq core2_stack
    
    cmp x5, #3
    beq core3_stack

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
    msr sp_el2, x4

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
    bl irq_init_vectors

    cmp x5, #MASTER_CORE
    bne slave_core_sleep

    adr x0, bss_begin
    adr x1, bss_end
    sub x1, x1, x0
    bl memzero
    bl kernel_main
    b hang

.balign 4
slave_core_sleep:
    wfe
	mov	x2, 204
	movk x2, 0x4000, lsl 16 //0x400000CC
	mrs x0, mpidr_el1
	ubfiz x0, x0, 4, 4
	ldr	w1, [x0, x2]
	cbz w1, slave_core_sleep
    str w1, [x0, x2]
    
    dmb sy // data memory buffer
    blr x1 //branch and link to register
    dmb sy
    b hang
    b slave_core_sleep

.globl core_execute
core_execute:
    dmb sy
    ubfiz x0, x0, 2, 8
    mov x2, 140
    movk x2, 0x4000, lsl 16
    str w1, [x2, x0, lsl 2]
    sev
    dmb sy
    ret
