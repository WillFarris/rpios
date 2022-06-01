.globl get_cntfrq_el0
get_cntfrq_el0:
    mrs x0, cntfrq_el0
    ret

.globl write_cntp_tval
write_cntp_tval:
    msr cntp_tval_el0, x0
    ret

.globl read_cntp_tval
read_cntp_tval:
    mrs x0, cntp_tval_el0
    ret

.globl write_cntp_tval_el0
write_cntp_tval_el0:
    msr cntp_tval_el0, x0
    ret

.globl read_cntpct_el0
read_cntpct_el0:
    mrs x0, cntpct_el0
    ret

.globl enable_cntp
enable_cntp:
    mov x0, #1
    msr cntp_ctl_el0, x0
    ret