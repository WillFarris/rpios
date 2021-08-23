#include "gpio.h"

.globl delay
delay:
    subs x0, x0, #1
    bne delay
    ret

.globl put32
put32:
    str w1, [x0]
    ret

.globl get32
get32:
    ldr w0, [x0]
    ret

.globl put64
put64:
    str x1, [x0]
    ret

.globl get64
get64:
    ldr x0, [x0]
    ret

.globl memzero
memzero:
    str xzr, [x0], #8
    subs x1, x1, #8
    bgt memzero
    ret

.globl get_el
get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    and x0, x0, #3
    ret

.globl get_core
get_core:
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    ret

.globl sys_timer_get_ticks
sys_timer_get_ticks:
    ldr	x0, =SYS_TIMER_ADDR
    add x0, x0, #4
    ldp	w0, w1, [x0]
    lsl x1, x1, #32
    orr x0, x0, x1
    ret

.globl panic
panic:
    b panic