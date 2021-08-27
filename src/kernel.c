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
    u8 core = get_core();
    u8 el = get_el();
    fbprintf("Hello world from core %d in EL%d\n", core, el);
}

extern void enter_el0(void *func);

void print_core_el0() {
    enter_el0(print_core);
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    fbinit(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    u32 bg = 0x800000;
    fbclear(bg);

    fbprintf("Booting Raspberry Pi 3\n\n");
    
    enable_interrupt_controller();
    fbprintf("Interrupt controller initialized\n");
    
    irq_enable();
    fbprintf("Enabled IRQ interrupts\n");
    
    //sys_timer_init();
    //fbprintf("Enabled system timer\n");
    
    //local_timer_init();
    //printf("Enabled local timer on core %d\n", get_core());
    
    fbprintf("\nFrameBuffer\n  width: %d\n  height: %d\n  pitch: %d\n  background: 0x%X\n  address: 0x%X\n\n", fb.width, fb.height, fb.pitch, fb.bg, fb.ptr);
    
    
    print_core();
    delay(10000);
    core_execute(1, print_core);
    delay(10000);
    core_execute(2, print_core);
    delay(50000);
    core_execute(3, print_core);

    while(1) {}

    //shell();
}