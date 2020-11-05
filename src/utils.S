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

.globl sys_timer_get_ticks
sys_timer_get_ticks:
    ldr	x0, =0x3f003004
    ldp	w0, w1, [x0]
    lsl x1, x1, #32
    orr x0, x0, x1
    ret