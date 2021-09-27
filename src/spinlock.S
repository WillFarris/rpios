.globl acquire
acquire:
spin_lock:
    mov    w2, 1
    .p2align 8
.L4:
    ldaxr  w1, [x0]
    stlxr  w3, w2, [x0]
    cbnz   w3, .L4
    cbnz   w1, .L4
    ret

.globl release
release:
    mov w2, 0
    str w2, [x0]
    dmb sy
    ret