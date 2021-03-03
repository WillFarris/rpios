#pragma once

#include "fb.h"

typedef struct {
    unsigned int magic;
    unsigned int version;
    unsigned int headersize;
    unsigned int flags;
    unsigned int numglyph;
    unsigned int bytesperglyph;
    unsigned int height;
    unsigned int width;
    unsigned char glyphs;
} __attribute__((packed)) psf_t;
extern volatile unsigned char _binary_font_psf_start;

void fbprint(char *s);
void fbputc_printf(void *p, char c);
void fbputc(char c);