#include "gfx.h"
#include "fb.h"
#include "types.h"
#include "pi_logo.h"

void draw_pi_logo(u32 x, u32 y) {
  //acquire(&fb.lock);
  u32 *in = pi_logo.pixel_data;
  u32 *out = (u32 *) fb.ptr + x + (fb.width * y);
  for(int row=0;row<pi_logo.height; ++row) {
    for(int col=0;col<pi_logo.width; ++col) {
      u32 raw = *in++;
      if(!raw || (out < fb.ptr)) {
        ++out;
        continue;
      }

      u32 r = (raw & 0xFF0000) >> 16;
      u32 g = (raw & 0x00FF00);
      u32 b = (raw & 0x0000FF) << 16;

      *out++ = r | g | b;
    }
    out += fb.width - pi_logo.width;
  }
  //release(&fb.lock);
}

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

u32 wheel(u8 pos) {
  pos = ~pos;
  //(r >> 16) | g | (b << 16);
  if(pos < 85) {
    return ((255 - pos * 3) << 16) | (0 << 8) | ((pos * 3) << 0);
  }
  if(pos < 170) {
    pos -= 85;
    return (0 << 16) | ((pos * 3) << 8) | ((255 - pos * 3) << 16);
    //return strip.Color(0, pos * 3, 255 - pos * 3)
  }
  pos -= 170;
  return ((pos * 3) << 16) | ((255 - pos * 3) << 8) | (0 << 0);
}