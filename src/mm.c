#include "mm.h"
#include "printf.h"
#include "types.h"

extern u8 kernel_heap[];
static u8 page_map[NUM_PAGES] = {0,};

extern u8 __kernel_heap_start;

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
