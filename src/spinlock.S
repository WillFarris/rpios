.globl acquire
acquire:
    mov x1, x0
    bl get_core

    ldaxr x3, [x2]
    cmp x3, #0
    bne acquire
    //stlxr x0, [x2]
    ret

.globl release
release:
    //stlxr xzr, [x0]
    ret