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
#include "process.h"

#include "reg.h"

struct FrameBuffer fb;


int printf_lock = 0;

void core_welcome() {
    u8 core = get_core();
    u64 sp = get_sp();

    fbprintf("Core %d online with sp=0x%X\n", core, sp);
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
    

    fbprintf("Here are the available cores:\n\n");
    
    core_welcome();
    delay(100000000);
    core_execute(1, core_welcome);
    delay(100000000);
    core_execute(2, core_welcome);
    delay(100000000);
    core_execute(3, core_welcome);

    while(1) {}

    //shell();
}