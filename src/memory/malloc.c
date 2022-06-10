#include "malloc.h"
#include "types.h"

#include "printf.h"

extern u8 __kernel_img_end;

const u64 SIZE_OF_PAGE =  65536;
const u64 NUM_PAGES = 128;

#define MAX_ORDER 32
#define POOLSIZE (1UL << MAX_ORDER)

struct _heap {
    u64 freelist[MAX_ORDER+2];
    u8 bytes[POOLSIZE];
} * heap = &__kernel_img_end;

void test_heap() {
    printf("Heap starts at 0x%X\n", heap);
    malloc(sizeof(unsigned long long));
}

void malloc_init() {
    //TODO: Initialize data structures
}

void * malloc(u64 size) {
    //TODO: Everything
    return NULL;
}