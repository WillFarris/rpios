#pragma once


#include "types.h"

u8 acquire(u8 *lock_addr);
u8 release(u8 *lock_addr);