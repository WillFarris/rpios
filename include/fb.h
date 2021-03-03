#pragma once

#include "types.h"

struct FrameBuffer
{
    u32 width;
    u32 height;
    u32 pitch;
    u32 isrgb;
    u32 cursor_x;
    u32 cursor_y;
    u8 * ptr;
};

i32 init_fb(int, int, int, int);
void clear(u32 color);