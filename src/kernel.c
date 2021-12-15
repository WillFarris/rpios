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
#include "schedule.h"
#include "shell.h"
#include "gfx.h"
#include "reg.h"
#include "proc.h"
#include "mm.h"

struct FrameBuffer fb;

void core_welcome() {
    u8 core = get_core();
    u64 sp = get_sp();
    printf("Core %d online with sp=0x%X\n", core, sp);
}

void loop_schedule() {
    printf("Starting scheduler on core %d\n", get_core());
    while(1) schedule();
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    fbinit(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    printf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");
    //fbprintf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");
    
    //enable_interrupt_controller();
    //fbprintf("Interrupt controller initialized\n");
    
    //sys_timer_init();
    //fbprintf("Enabled system timer\n");

    /*reg32 local_timer_core = 0;
    local_timer_init(local_timer_core, 0); // route to core local_timer_core IRQ (fiq = false)
    printf("Enabled local timer routing to core %d\n", local_timer_core);*/
    
    //printf("\nFrameBuffer\n  width: %d\n  height: %d\n  pitch: %d\n  background: r=%d, g=%d, b=%d\n  address: 0x%X\n\n", fb.width, fb.height, fb.pitch, red, green, blue, fb.ptr);

    mmu_init();

    QA7->control_register = 0b00 << 8;

    init_scheduler();
    core_timer_init();

    //new_process((u64) print_sys_info, 0, "sys_info");
    new_process((u64) draw_rects, 0, "raspberry_pi_logo");
    new_process((u64) shell, 0, "shell");

    //core_execute(1, loop_schedule);
    //core_execute(2, loop_schedule);
    //core_execute(3, loop_schedule);

    new_process((u64) test_process, 420, "test420");
    new_process((u64) test_process, 69, "test69");

    
    loop_schedule();
}
