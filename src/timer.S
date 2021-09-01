.globl get_cntfrq_el0
get_cntfrq_el0:
    mrs x0, cntfrq_el0
    ret

.globl write_cntv_tval
write_cntv_tval:
    msr cntv_tval_el0, x0
    ret

.globl read_cntv_tval
read_cntv_tval:
    mrs x0, cntv_tval_el0
    ret

.globl enable_cntv
enable_cntv:
    mov x0, #1
    msr cntv_ctl_el0, x0
    ret

.globl read_cntvct
read_cntvct:
    mrs x0, cntvct_el0
    ret