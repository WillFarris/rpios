.globl acquire
acquire:
.p2align 8
    mrs     x4, mpidr_el1
    and     x4, x4, 0xFF
    add     x4, x4, #1
.L4:
    ldaxr   w1, [x0]
    cbnz    w1, .L4
    mov     w2, w4
    stlxr   w3, w2, [x0]
    cbnz    w3, .L4
    
    mov     x0, x4
    dmb     sy
    dc      civac, x0
    ret

.globl release
release:
    mov     x2, 0
    ldaxr   x1, [x0]
    stlxr   w3, w2, [x0]
    cbnz    w3, release
    dmb     sy
    dc      civac, x0
    ret