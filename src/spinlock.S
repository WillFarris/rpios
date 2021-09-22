.globl acquire
acquire:
spin_lock:
    mov    w2, 1
    .p2align 2
acqloop:
    ldaxrb    w1, [x0]
    dmb sy
    stxrb    w3, w2, [x0]
    dmb sy
    cbnz    w3, acqloop
    uxtb    w1, w1
    cbnz    w1, acqloop
    ret

.globl release
release:
    mov w2, 0
    mov w1, #0xFF
releaseloop:
    //stxrb w1, w2, [x0]
    str w2, [x0]
    //cbnz w1, releaseloop
    ret