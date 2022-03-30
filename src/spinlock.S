.globl acquire
acquire:
spin_lock:
    .p2align 8
.L4:
    ldaxr  w1, [x0]
    mov    w2, #1
    stlxr  w3, w2, [x0]
    cbnz   w3, .L4
    cbnz   w1, .L4
    dmb sy
    ret

.globl release
release:
    dmb sy
    mov w2, 0
    ldaxr  w1, [x0]
    stlxr w3, w2, [x0]
    cbnz w3, release
    dmb sy
    ret