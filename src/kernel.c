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

    printf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");
    
    mmu_init();
    enable_interrupt_controller();
    printf("Interrupt controller initialized\n");

    QA7->control_register = 0b00 << 8;

    init_scheduler();
    core_timer_init();

    new_process((u64) shell, 0, "shell");

    //core_execute(1, core_timer_init);
    //core_execute(1, loop_schedule);
    //core_execute(2, loop_schedule);
    //core_execute(3, loop_schedule);

    loop_schedule();
}
