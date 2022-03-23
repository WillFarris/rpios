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

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    printf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");
    
    mmu_init();
    enable_interrupt_controller();
    printf("Interrupt controller initialized\n");

    QA7->control_register = 0b00 << 8;

    init_ptable();
    new_process((u64) shell, "shell", 0, NULL);

    core_execute(1, start_scheduler);
    core_execute(2, start_scheduler);
    core_execute(3, start_scheduler);
    
    start_scheduler();
}
