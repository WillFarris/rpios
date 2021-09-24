.globl acquire
acquire:
    ldxrb  w8, [x0]
    cbnz  w8, acquire
    add   w8, w8, #1  // =1
    stlxrb  w9, w8, [x0]
    cbnz  w9, acquire
    dmb ish
    ret

.globl release
release:
    mov w2, 0
    mov w1, #0xFF
releaseloop:
    //stxrb w1, w2, [x0]
    strb w2, [x0]
    //cbnz w1, releaseloop
    ret