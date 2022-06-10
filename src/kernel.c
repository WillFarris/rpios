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
#include "malloc.h"

struct FrameBuffer fb;
extern struct _ptable ptable;
extern struct lock_table {
    u32 ptable_lock;
    u32 mem_map_lock;
    u32 counter_lock;
} locks;

void smp_scheduler() {
    mmu_init();
    start_scheduler();
    core_timer_init();
    irq_enable();
    while(1) {
        schedule();
    }
}

void kernel_main()  {
    uart_init_alt();    
    init_printf(0, putc);

    uart_puts("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");
    

    locks.ptable_lock = 0;
    locks.mem_map_lock = 0;
    locks.counter_lock = 0;

    fbinit(1280, 720);

    init_page_tables(&locks);
    mmu_init();
    irq_disable();

    QA7->control_register = 0b00 << 8;

    test_heap();

    init_ptable(&locks.ptable_lock);
    new_process((u64) shell, "shell", 0, NULL);
    new_process((u64) rainbow_square, "colors", 3, (char *[3]) {"draw_pi_logo", "128", "0"});
    new_process((u64) rainbow_square, "colors", 3, (char *[3]) {"draw_pi_logo", "128", "32"});
    new_process((u64) rainbow_square, "colors", 3, (char *[3]) {"draw_pi_logo", "128", "64"});
    new_process((u64) rainbow_square, "colors", 3, (char *[3]) {"draw_pi_logo", "128", "96"});

    core_execute(1, smp_scheduler);
    core_execute(2, smp_scheduler);
    core_execute(3, smp_scheduler);
    smp_scheduler();   
}
