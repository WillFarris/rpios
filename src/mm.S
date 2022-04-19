.globl get_id_aa64mmfr0_el1
get_id_aa64mmfr0_el1:
    mrs x0, id_aa64mmfr0_el1
    ret

.globl set_mair_el1
set_mair_el1:
    msr mair_el1, x0
    ret

.globl mmu_init_asm
mmu_init_asm:
    msr tcr_el1, x2
    isb

    msr ttbr0_el1, x0
    msr ttbr1_el1, x1
    isb

    //0001 0000 0000 0101 = 4101 = i-cache, d-cache, mmu enable bits
    mrs x0, sctlr_el1
    mov x1, #4101
    orr x0, x0, x1
    msr sctlr_el1, x0
    isb

    mov x0, #0
    tlbi vmalle1
    dsb
    isb

    ret

.globl flush_cache
flush_cache:
    dc civac, x0
    dsb ish
    ic ivau, x0
    dsb ish
    isb
    ret
