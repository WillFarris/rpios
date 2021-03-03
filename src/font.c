#include "font.h"
#include "fb.h"

extern struct FrameBuffer fb;

/**
 * Display a string using fixed size PSF
 */
void fbprint(char *s)
{
    // get our font
    psf_t *font = (psf_t*)&_binary_font_psf_start;
    // draw next character if it's not zero
    while(*s) {
        // get the offset of the glyph. Need to adjust this to support unicode table
        unsigned char *glyph = (unsigned char*)&_binary_font_psf_start +
         font->headersize + (*((unsigned char*)s)<font->numglyph?*s:0)*font->bytesperglyph;
        // calculate the offset on screen
        int offs = (fb.cursor_y * fb.pitch) + (fb.cursor_x * 4);
        // variables
        int i,j, line,mask, bytesperline=(font->width+7)/8;
        // handle carrige return
        if(*s == '\r') {
            fb.cursor_x = 0;
        } else
        // new line
        if(*s == '\n') {
            fb.cursor_x = 0; fb.cursor_y += font->height;
        } else {
            // display a character
            for(j=0;j<font->height;j++){
                // display one row
                line=offs;
                mask=1<<(font->width-1);
                for(i=0;i<font->width;i++){
                    // if bit set, we use white color, otherwise transparent background
                    u32 col = *(fb.ptr + line);
                    *((u32*)(fb.ptr + line))=((int)*glyph) & mask?0xFFFFFF:fb.bg;
                    mask>>=1;
                    line+=4;
                }
                // adjust to next line
                glyph+=bytesperline;
                offs+=fb.pitch;
            }
            fb.cursor_x += (font->width+1);
        }
        // next character
        s++;
    }
}

void fbputc_printf(void *p, char c)
{
    fbputc(c);
}

void fbputc(char c)
{
    // get our font
    psf_t *font = (psf_t*)&_binary_font_psf_start;
    // get the offset of the glyph. Need to adjust this to support unicode table
    unsigned char *glyph = (unsigned char*)&_binary_font_psf_start +
    font->headersize + (c < font->numglyph ? c : 0)*font->bytesperglyph;
    // calculate the offset on screen
    int offs = (fb.cursor_y * fb.pitch) + (fb.cursor_x * 4);
    // variables
    int i,j, line,mask, bytesperline=(font->width+7)/8;
    // handle carrige return
    if(c == '\r') {
        fb.cursor_x = 0;
    } else
    // new line
    if(c == '\n') {
        fb.cursor_x = 0; fb.cursor_y += font->height;
    } else {
        // display a character
        for(j=0;j<font->height;j++){
            // display one row
            line=offs;
            mask=1<<(font->width-1);
            for(i=0;i<font->width;i++){
                // if bit set, we use white color, otherwise transparent background
                u32 col = *(fb.ptr + line);
                *((u32*)(fb.ptr + line))=((int)*glyph) & mask?0xFFFFFF:col;
                mask>>=1;
                line+=4;
            }
            // adjust to next line
            glyph+=bytesperline;
            offs+=fb.pitch;
        }
        fb.cursor_x += (font->width+1);
    }
}