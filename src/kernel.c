#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "fb.h"
#include "font.h"
#include "timer.h"
#include "regstruct.h"

struct FrameBuffer fb;

void kernel_main() 
{
    uart_init_alt();    
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
    
    local_timer_init();
    printf("Enabled local timer\n");
    
    int fb_status = fbinit(800, 600, 800, 600);
    if(fb_status == 0)
        printf("Initialized framebuffer of size %dx%d\n", fb.width, fb.height);
    else
        printf("Could not initialize framebuffer.\n");
    
    

    //u32 color = 0x03c6fc;
    u32 color = 0x800000;
    fbclear(color);
    fbprint("Hello world!\nSetup done. Running main kernel loop.\n\n");

    
    const char* running = "Running..";
    int len = strlen(running);
    char *c = running;
    while(1)
    {
        /*if(c < (running+len))
            uart_putc(*c++);
        else
        {
            c = running;
            printf("\r           \r");
        }
        sys_timer_sleep_ms(100);*/
    }
    
}