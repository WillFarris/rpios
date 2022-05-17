.globl acquire
acquire:
.p2align 8
    mov x2, #1
.p2align 2
.L4:
    ldaxrb  w1, [x0]
    stlxrb  w3, w2, [x0]
    cbnz    w3, .L4
    uxtb    w1, w1
    cbnz    w1, .L4
    ret

.globl release
release:
    stlr   wzr, [x0]
    ret

.globl atomic_set
atomic_set:
    ldaxr   w2, [x1]
    stlxr   w3, w0, [x1]
    mov     w0, w3
    ret
