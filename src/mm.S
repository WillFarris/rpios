.globl get_id_aa64mmfr0_el1
get_id_aa64mmfr0_el1:
    mrs x0, id_aa64mmfr0_el1
    ret

.globl set_mair_el1_asm
set_mair_el1_asm:
    msr mair_el1, x0
    ret

.globl mmu_init_asm
mmu_init_asm:
    msr ttbr0_el1, x0
    msr ttbr1_el1, x1
    msr tcr_el1, x2
    isb

    mrs x0, sctlr_el1
    orr x0, x0, #1
    msr sctlr_el1, x0
    isb
    ret