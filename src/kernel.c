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
#include "shell.h"

#include "reg.h"

struct FrameBuffer fb;

void core_welcome() {
    u8 core = get_core();
    u64 sp = get_sp();
    fbprintf("Core %d online with sp=0x%X\n", core, sp);
}

void test69() {
    printf("test\n");
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
    
    sys_timer_init();
    fbprintf("Enabled system timer\n");

    //reg32 local_timer_core = 3;
    //local_timer_init(local_timer_core, 0); // route to core local_timer_core IRQ (fiq = false)
    //fbprintf("Enabled local timer routing to core %d\n", local_timer_core);
    
    u8 red = fb.bg >> 16 & 0xFF;
    u8 green = fb.bg >> 8 & 0xFF;
    u8 blue = fb.bg & 0xFF;
    fbprintf("\nFrameBuffer\n  width: %d\n  height: %d\n  pitch: %d\n  background: r=%d, g=%d, b=%d\n  address: 0x%X\n\n", fb.width, fb.height, fb.pitch, red, green, blue, fb.ptr);
    
    fbprintf("Here are the available cores:\n\n");
    core_welcome();
    sys_timer_sleep_ms(100);
    core_execute(1, core_welcome);
    sys_timer_sleep_ms(100);
    core_execute(2, core_welcome);
    sys_timer_sleep_ms(100);
    core_execute(3, core_welcome);

    core_execute(3, test69);

    core_execute(3, shell);


    while(1) {
        wfe();
    }
}