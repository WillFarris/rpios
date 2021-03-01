#pragma once

#include "types.h"

struct FrameBuffer
{
    u32 width;
    u32 height;
    u32 pitch;
    u32 isrgb;
    u8 * ptr;
};

i32 init_fb(struct FrameBuffer *, int, int, int, int);