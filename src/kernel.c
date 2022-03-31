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

void secondary_startup() {
    mmu_init();
    start_scheduler();
}
extern struct _ptable ptable;

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    printf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");

    init_page_tables();
    mmu_init();
    init_ptable();

    QA7->control_register = 0b00 << 8;

    //new_process((u64) test_loop, "test_loop", 0, NULL);

    core_execute(1, secondary_startup);
    core_execute(2, secondary_startup);
    core_execute(3, secondary_startup);

    //local_timer_init(1, 0);
    //local_timer_init(2, 0);
    //local_timer_init(3, 0);

    new_process((u64) test_loop, "test_loop", 0, NULL);
    new_process((u64) test_loop, "test_loop", 0, NULL);
    new_process((u64) shell, "shell", 0, NULL);

    core_timer_init();

    start_scheduler();


    while(1) {}
}
