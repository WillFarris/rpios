#pragma once

#include "types.h"

//extern const u64 __kernel_heap_size;

#define PAGE_SIZE 4096
#define NUM_PAGES 16777216/PAGE_SIZE
//#define NUM_PAGES __kernel_heap_size / PAGE_SIZE

u64 get_free_page();
void free_page(u64 *);
