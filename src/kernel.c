#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "fb.h"
#include "timer.h"
#include "regstruct.h"

void kernel_main() 
{
    uart_init();
    init_printf(0, putc);

    printf("\n\nBooting Raspberry Pi 3\n");
    
    printf("\nRunning on core %d\nWe are in EL%d\n\n", get_core(), get_el());

    irq_init_vectors();
    printf("IRQ vector table initalized\n");
    
    enable_interrupt_controller();
    printf("Interrupt controller initialized\n");
    
    irq_enable();
    printf("Enabled IRQ interrupts\n");
    
    sys_timer_init();
    printf("Enabled system timer\n");
    
    //local_timer_init();
    //printf("Enabled local timer\n");
    
    struct FrameBuffer fb;
    int fb_status = init_fb(&fb, 1024, 600, 1024, 600);
    if(fb_status == 0)
        printf("Initialized framebuffer of size %dx%d\n", fb.width, fb.height);
    else
        printf("Could not initialize framebuffer.\n");

    //u32 color = 0x03c6fc;
    u32 color = 0xfcc603;
    u32 * cur_pixel = fb.ptr;
    for(int y=0; y<fb.height;++y)
    {
        for(int x=0;x < fb.width;++x)
        {
            if(!fb.isrgb)
            {
                u32 r = color & 0xFF0000 >> 16;
                u32 b = color & 0x0000FF;
                u32 g = color & 0x00FF00 >> 8;
                color = r << 16 | g | b;
            }
            *cur_pixel = color;
            cur_pixel++;
        }
        cur_pixel += fb.pitch - fb.width*4;
    }

    printf("\nSetup done. Running main kernel loop.\n\n");
    
    const char* running = "Running..";
    int len = strlen(running);
    char *c = running;
    while(1)
    {
        if(c < (running+len))
            uart_putc(*c++);
        else
        {
            c = running;
            printf("\r           \r");
        }
        sys_timer_sleep_ms(100);        
    }
    
}