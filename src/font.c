#include "font.h"
#include "fb.h"
#include "printf.h"

void fbputs(const char*s)
{
    if(!s) return;
    while(*s)
        fbputc(*s++);
}

void pf_fbputc(void *p, char c)
{
    if(c == '\n')
    {
        fbputc('\r');
    }
    fbputc(c);
}

void fbputc(char c)
{
    u8 core = get_core();
    if(c == '\n')
    {
        fb.cursor_y[core] += char_height;
    } else if (c == '\r')
    {
        fb.cursor_x[core] = 0;
    } else
    {
        if(fb.cursor_x[core] >= (fb.width - char_width))
        {
            fbputc('\n');
            fbputc('\r');
        }
        u32 bg = fb.bg;
        c -= 0x20;
        int cy = c / cols;
        int cx = c % cols;
        char *data = fontmap_data + (cy * fontmap_width * char_height) + (cx * char_width);
        char pixel[4];
        u32 * cur_pixel = (u32 *) fb.ptr + fb.cursor_x[core] + (fb.width * fb.cursor_y[core]);
        for(int y=0; y<char_height;++y)
        {
            for(int x=0;x < char_width;++x)
            {
                HEADER_PIXEL(data, pixel);
                u32 val = *((u32 *)&pixel);
                if(val != 0)
                    *cur_pixel = val;
                else
                    *cur_pixel = bg;
                cur_pixel++;
            }
            data += fontmap_width - char_width;
            cur_pixel += fb.width - char_width;
        }
        if(fb.cursor_x[core] < (fb.width - char_width))
            fb.cursor_x[core] += char_width;
        else
        {
            fb.cursor_x[core] = 0;
            fb.cursor_y[core] += char_height;
        }
    }

    if(fb.cursor_y[core]+char_height > fb.height) {
        fbclear(fb.bg);
        fb.cursor_y[core] = 0;
    }
}