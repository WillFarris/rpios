#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "fb.h"
#include "font.h"
#include "timer.h"
#include "regstruct.h"

#include "mem.h"
#include "reg.h"

struct FrameBuffer fb;

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    int fb_status = fbinit(1024, 600);
    fb.bg = 0x800000;
    fbclear(fb.bg);

    fbprintf("Booted Raspberry Pi 3\n");
    
    fbprintf("\nRunning on core %d\nWe are in EL%d\n\n", get_core(), get_el());

    irq_init_vectors();
    fbprintf("IRQ vector table initalized\n");
    
    enable_interrupt_controller();
    fbprintf("Interrupt controller initialized\n");
    
    irq_enable();
    fbprintf("Enabled IRQ interrupts\n");
    
    sys_timer_init();
    fbprintf("Enabled system timer\n");
    
    local_timer_init();
    printf("Enabled local timer on core %d\n", get_core());

    printf("FrameBuffer:\n  width: %d\n  height: %d\n  pitch: %d\n  isrgb: %d\n  ptr: %X\n", fb.width, fb.height, fb.pitch, fb.isrgb, fb.ptr);

    fbprintf("\nStack starts at: 0x%X\n", LOW_MEMORY);

    fbprintf("\nSetup done. Running main kernel loop.\n\n");
    
    const char* running = "Running..";
    int len = strlen(running);
    char *c = running;
    int dy = 0;
    while(1)
    {
        /*if(c < (running+len))
            fbputc(*c++);
        else
        {
            c = running;
            fbputs("\r           \r");
        }
        sys_timer_sleep_ms(100);*/
    }
}