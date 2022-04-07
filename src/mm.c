#include "mm.h"
#include "gpio.h"
#include "printf.h"
#include "types.h"
#include "utils.h"

// SHIFT = log2(size)
// NUM_LVL2_TABLES = KernelAddressSpaceSize >> Granule512MiBShift
//                 = (0x4000_FFFF + 1) >> log2()
// Identity map address space up to and including Pi 3 peripherals
//#define NUM_TABLES ((0x4000FFFF + 1) >> 30)
#define NUM_TABLES 3//((0xFFFFFFFF + 1) >> 30)
const u64 num_tables = NUM_TABLES;

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

void print_pa_range_support() {
    u8 ips = (u8)(get_id_aa64mmfr0_el1() & 0xF);
    printf("Physical address range supported: %d bits\n",
        ips == 0 ? 32 : ips == 1 ? 36 : ips == 2 ? 40 : ips == 3 ? 42 : ips == 4 ? 44 : ips == 5 ? 48 : ips == 6 ? 52 : 0xFF
    );
}

void init_page_tables(u8 * locks_page_addr) {
    printf("Populating %d level 2 translation tables\n", NUM_TABLES);
    for(int i=0;i<NUM_TABLES;i++) {
        u64 lvl2_addr = (u64)&(translation_table.lower_level3[i]) >> 16;
        translation_table.lower_level2[i] = (
            (lvl2_addr << 16) | // next level table address
            (0b11      <<  0)   // table, valid
        );
        
        for(int j=0;j<8192;++j) {
            u64 virt_addr = (i << 29) + (j << 16);
            u64 mair_attr = 4;

            // PBASE (start of MMIO) is 0x3F000000 on Pi 3
            // MMIO used by this program ends at 0x4000FFFF
            // Set device memory to use MAIR_EL1 attribute 0
            if(virt_addr >= PBASE && virt_addr <= 0x4000FFFF) {
                //printf("[lvl2 table %d] Mapping device memory at 0x%X vs 0x%X\n", i, virt_addr, PBASE);
                mair_attr = 0;
            } else if (virt_addr == locks_page_addr) {
                // Mark the page containing our mutex/semaphores as write-back to appease some weird hardware requirement in the manual
                printf("[lvl2 table %d] Mapping lock structure at 0x%X vs 0x%X\n", i, virt_addr, locks_page_addr);
                mair_attr = 1;
            }
            
            translation_table.lower_level3[i][j] = (
                virt_addr         | // Virtual address
                (0b1UL     << 10) | // Accessed
                (0b11      <<  8) | // Inner-sharable
                (0b0       <<  7) | // Read-Write
                (0b0       <<  6) | // Kernel only
                (mair_attr <<  2) | // MAIR attribute index
                (0b11      <<  0)   // valid page
            );
            //printf("Last entry in L3 table for L2 table %d at index 0x%X: 0x%X\n", i, j, translation_table.lower_level3[i][j]);
        }
    }
}

void mmu_init() {
    u8 core = get_core();
    printf("[core %d] Turning on MMU\n", core);
    //u8 ips = (u8)(get_id_aa64mmfr0_el1() & 0xF);
    //u64 t0sz = 64 - 30;
    //print_pa_range_support(ips);
    u64 tcr_el1 =
        (0b0LL      << 37) | // TBI0,  Top byte used in address calculation
        (0b010LL    << 32) | // IPS,   40 bit virtual address
        (0b01LL     << 14) | // TG0,   64KiB
        (0b11LL     << 12) | // SH0,   Inner-sharable
        (0b00LL     << 10) | // ORGN0, Write back read-alloc write-alloc cacheable
        (0b00LL     <<  8) | // IRGN0, Write back read-alloc write-alloc cacheable
        (0b0LL      <<  7) | // EPD0,  Enable TTBR0 walks
        (0b1LL      << 22) | // A1,    TTBR0
        (32LL       <<  0) | // T0SZ, 
        (0b1LL      << 23);  // EDP1,  Disable TTBR1 walks

    // Set MAIR attributes
    // 1: 0xFF - regular DRAM
    // 0: 0x04 - device memory
    set_mair_el1(
        (0b01000100 << 32) | // Attr 4: Normal memory, Inner + Outer non-cacheable
        (0b10111011 << 24) | // Attr 3: Normal memory, Inner + Outer write-through non-transient, RW
        (0b00001100 << 16) | // Attr 2: Device memory, GRE
        (0b11111111 <<  8) | // Attr 1: Normal memory, Outer Write-Back Non-transient, RW
        (0b00000000 <<  0)   // Attr 0: Device memory, nGnRnE
    );

    u64 ttbr1_el1 = (u64) &translation_table.higher_level2;
    u64 ttbr0_el1 = (u64) &translation_table.lower_level2;
    mmu_init_asm(ttbr0_el1, ttbr1_el1, tcr_el1);
    printf("[core %d] MMU enabled\n", core);
}

extern struct lock_table {
    u64 ptable_lock;
    u64 mem_map_lock;
    u64 test_lock;
} locks;

u64 get_free_page() {
    acquire(&locks.mem_map_lock);
    for(int i=0;i<NUM_PAGES;++i)
    {
        if(page_map[i] == 0)
        {
            page_map[i] = 1;
            flush_cache(&page_map[i]);

            u64 page_addr = &__kernel_heap_start + (i * PAGE_SIZE);
            printf("Allocating page at 0x%X, index %d\n", page_addr, i);
            for(u64 j=0;j<PAGE_SIZE/8;++j) {
                *((u64*) page_addr+j) = 0;
            }
            release(&locks.mem_map_lock);
            return page_addr;
        }
    }
    printf("Could not allocate page\n");
    release(&locks.mem_map_lock);
    return 0;
}

void free_page(void * page) {
    acquire(&locks.mem_map_lock);
    u64 index = ((u64) (page - (void*)&__kernel_heap_start)) / PAGE_SIZE;
    printf("Freeing page at 0x%X, index %d\n", page, index);
    page_map[index] = 0;
    void *addr = page;
    /*for(u64 j=0;j<PAGE_SIZE/8;++j) {
        *((u64*)addr+j) = 0;
    }*/
    flush_cache(&page_map[index]);
    release(&locks.mem_map_lock);
}
