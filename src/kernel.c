#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "fb.h"
#include "font.h"
#include "timer.h"
#include "regstruct.h"
#include "string.h"
#include "math.h"

#include "mem.h"
#include "reg.h"

#define DISPLAY_WIDTH 1024
#define DISPLAY_HEIGHT 600;

struct FrameBuffer fb;
char commandbuffer[DISPLAY_WIDTH];
char * args[10];
char *cur;

#define cmd_match(var, cmd) (strcmp(var, cmd) == 0)

void exec()
{
    int i=0;
    while(args[i])
    {
        printf("Arg %d: %s\n", i, args[i]);
        ++i;
    }
    if(strcmp(args[0], "gcd") == 0)
    {
        u64 a = strtol(args[1]);
        u64 b = strtol(args[2]);
        fbprintf("%d\n> ", gcd(a, b));
    } else if(strcmp(args[0], "phi") == 0)
    {
        u64 n = strtol(args[1]);
        fbprintf("%d\n> ", phi(n));
    } else if(strcmp(args[0], "clear") == 0)
    {
        fbclear(0x800000);
        fbprintf("> ");
    } else if(strcmp(args[0], "mod") == 0)
    {
        u64 a = strtol(args[1]);
        u64 b = strtol(args[2]);
        fbprintf("%d\n> ", a % b);
    } else
    {
        fbprintf("Unknown command: %s\n> ", args[0]);
    }
}

void command(char *str)
{
    int i=0;
    char *argstart = str;

    char * cur = str;
    while(*cur)
    {
        if(i > 9) return;
        if(*cur == ' ')
        {
            args[i++] = argstart;
            *cur = 0;
            argstart = ++cur;
        } else ++cur;
    }
    args[i++] = argstart;
    args[i] = 0;
    exec();
}

void shell()
{
    char c = uart_getc();
    switch(c)
    {
        case 0x7F:
            fb.cursor_x -= char_width;
            *(--cur) = 0;
            --cur;
            fbputc(' ');
            fb.cursor_x -= char_width;
            break;
        case '\r':
            fbputc('\n');
            fbputc('\r');
            fbputc('>');
            fbputc(' ');
            *cur = 0;
            cur = commandbuffer;
            command(commandbuffer);
            break;
        default:
            fbputc(c);
            *cur = c;
            break;
    }
    if(c != '\r') ++cur;
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    int fb_status = fbinit(1024, 600);
    u32 bg = 0x800000;
    fbclear(bg);

    fbprintf("Booted Raspberry Pi 3 on core %d\nWe are in EL%d\n\n", get_core(), get_el());

    irq_init_vectors();
    printf("IRQ vector table initalized\n");
    
    enable_interrupt_controller();
    printf("Interrupt controller initialized\n");
    
    irq_enable();
    printf("Enabled IRQ interrupts\n");
    
    //sys_timer_init();
    //fbprintf("Enabled system timer\n");
    
    //local_timer_init();
    //printf("Enabled local timer on core %d\n", get_core());
    
    printf("\nFrameBuffer:\n  width: %d\n  height: %d\n  pitch: %d\n  background: 0x%X\n  address: 0x%X\n", fb.width, fb.height, fb.pitch, fb.bg, fb.ptr);
    
    fbprintf("\n> ");
    
    cur = commandbuffer;

    while(1);
}