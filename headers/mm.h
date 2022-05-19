#pragma once

#include "types.h"


//#define PAGE_SIZE 4096
#define PAGE_SIZE 65536
#define NUM_PAGES 16777216/PAGE_SIZE


u64 get_free_page();
void free_page(void *);

void init_page_tables(u8 * locks_page_addr);
void mmu_init_asm();
void mmu_init();
void flush_cache(u64 address);
u64 get_id_aa64mmfr0_el1();

