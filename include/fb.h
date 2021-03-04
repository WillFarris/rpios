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
    u32 bg;
    u8 * ptr;
};

i32 fbinit(int, int);
void fbclear(u32 color);
void fbrainbow();

extern struct FrameBuffer fb;