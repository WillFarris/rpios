#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "fb.h"
#include "font.h"
#include "timer.h"
#include "regstruct.h"
#include "math.h"

#include "reg.h"

struct FrameBuffer fb;

void print_core() {
    printf("Hello world from core %d\n", get_core());
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    int fb_status = fbinit(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    u32 bg = 0x800000;
    fbclear(bg);

    printf("Booted Raspberry Pi 3 on core %d\nWe are in EL%d\n\n", get_core(), get_el());
    
    enable_interrupt_controller();
    printf("Interrupt controller initialized\n");
    
    irq_enable();
    printf("Enabled IRQ interrupts\n");
    
    sys_timer_init();
    fbprintf("Enabled system timer\n");
    
    //local_timer_init();
    //printf("Enabled local timer on core %d\n", get_core());
    
    printf("\nFrameBuffer\n  width: %d\n  height: %d\n  pitch: %d\n  background: 0x%X\n  address: 0x%X\n\n", fb.width, fb.height, fb.pitch, fb.bg, fb.ptr);
    

    core_execute(1, print_core);

    shell();
}