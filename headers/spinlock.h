#pragma once


#include "types.h"

u8 acquire(u64 *lock_addr);
u8 release(u64 *lock_addr);