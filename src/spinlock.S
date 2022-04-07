.globl acquire
acquire:
spin_lock:
    .p2align 8
.L4:
    mrs x4, mpidr_el1
    and x4, x4, 0xFF
    add x4, x4, #1

    ldaxrh  w1, [x0]
    mov    w2, w4
    stlxrh  w3, w2, [x0]
    cbnz   w3, .L4
    cbnz   w1, .L4

    mov x0, x4
    dmb sy
    ret

.globl release
release:
    mov w2, 0
    ldaxrh  w1, [x0]
    stlxrh w3, w2, [x0]
    cbnz w3, release
    dmb sy
    ret