#pragma once

#include "types.h"

void delay(u64);
void put32(u32 *, u32);
u32 get32(u32 *);
void put64(u64 *, u64);
u32 get64(u64 *);
void memzero(u64, u64);
u32 get_el();
u64 get_core();
void wfe();

u64 strlen();