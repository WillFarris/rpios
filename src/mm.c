#include "mm.h"
#include "printf.h"
#include "types.h"

struct {
    u64 lower_level3[NUM_TABLES][8192];
    u64 higher_level3[NUM_TABLES][8192];
    u64 lower_level2[NUM_TABLES];
    u64 higher_level2[NUM_TABLES];
} translation_table __attribute__((aligned(65536)));


extern u8 kernel_heap[];
static u8 page_map[NUM_PAGES] = {0,};
extern u8 __kernel_heap_start;
extern u8 __kernel_heap_end;
extern u8 __kernel_img_end;

u64 set_mair_el1(u8 value, u64 attr_num) {
    set_mair_el1_asm(((u64)value) << (8 * attr_num));
}

void mmu_init() {
    u8 ips = (u8)(get_id_aa64mmfr0_el1() & 0xF);
    u64 tcr_el1 =
        (0b00LL << 37) | // TBI=0, no tagging
        (ips    << 32) | // IPS=autodetected
        (0b10LL << 30) | // TG1=4k
        (0b11LL << 28) | // SH1=3 inner
        (0b01LL << 26) | // ORGN1=1 write back
        (0b01LL << 24) | // IRGN1=1 write back
        (0b0LL  << 23) | // EPD1 enable higher half
        (25LL   << 16) | // T1SZ=25, 3 levels (512G)
        (0b01LL << 14) | // TG0=64k
        (0b10LL << 12) | // SH0=2 outer
        (0b01LL << 10) | // ORGN0=1 write back
        (0b01LL << 8)  | // IRGN0=1 write back
        (0b0LL  << 7)  | // EPD0 enable lower half
        (25LL   << 0);   // T0SZ=25, 3 levels (512G)

    printf("Populating translation tables\n");
    for(int i=0;i<NUM_TABLES;++i) {
        //0xFFFF_FFFF = END_INCLUSIVE, shift = 32
        //0x4000_FFFF = END_INCLUSIVE, shift = 30

        u64 lvl2_addr = (u64)&(translation_table.lower_level3[i]) >> 16;
        translation_table.lower_level2[i] = (
            (lvl2_addr << 16) | // next level table address
            (0b1       <<  1) | // table 
            (0b1       <<  0)   // valid 
        ); // fill in the table descriptor
        
        for(int j=0;j<8192;++j) {
            u64 virt_addr = (i << 30) + (j << 16);

            translation_table.lower_level3[i][j] = (
                virt_addr |
                0 // set some flags here
            ); // fill each entry in the table
        }
    }

    u64 ttbr1_el1 = (u64) &translation_table.higher_level2;
    u64 ttbr0_el1 = (u64) &translation_table.lower_level2;
    printf("Setting tcr and enabling MMU...\n0x%X\n", &__kernel_img_end);
    mmu_init_asm(ttbr0_el1, ttbr1_el1, tcr_el1);
}

u64 get_free_page()
{
    for(int i=0;i<NUM_PAGES;++i)
    {
        if(page_map[i] == 0)
        {
            page_map[i] = 1;

            u64 page_addr = &__kernel_heap_start + (i * PAGE_SIZE);
            for(u64 i=0;i<PAGE_SIZE;++i) {
                *((u64*) page_addr+i) = 0;
            }
            return page_addr;
        }
    }
    return 0;
}

void free_page(void * page)
{
    page_map[((u64) (page - (void*)&__kernel_heap_start)) / PAGE_SIZE] = 0;
}
