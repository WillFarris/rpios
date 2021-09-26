#pragma once

#include "types.h"


#define PAGE_SIZE 4096
#define NUM_PAGES 16777216/PAGE_SIZE


u64 get_free_page();
void free_page(void *);
void mmu_init_asm();
void mmu_init();
u64 get_id_aa64mmfr0_el1();

// SHIFT = log2(size)
// NUM_LVL2_TABLES = KernelAddressSpaceSize >> Granule512MiBShift
//                 = (0x4000_FFFF + 1) >> log2()
#define NUM_TABLES ((0x4000FFFF + 1) >> 30)

