#include "gfx.h"
#include "fb.h"
#include "types.h"

void drawRect(u32 x, u32 y, u32 w, u32 h, u32 color) {
    u32 * cur_pixel = (u32 *) fb.ptr + x + (fb.width * y);
    for(int y=0; y<h;++y)
        {
            for(int x=0;x < w;++x)
            {
                
                *cur_pixel = color;
                cur_pixel++;
            }
            cur_pixel += fb.width - w;
        }
}