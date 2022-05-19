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
    printf("[core %d] Initialized lock structure at address 0x%X\n", get_core(), &locks);

    fbinit(1280, 720);

    init_page_tables(&locks);
    u8 ips = (u8)(get_id_aa64mmfr0_el1() & 0xF);
    print_pa_range_support(ips);
    mmu_init();
    irq_disable();

    QA7->control_register = 0b00 << 8;

    init_ptable(&locks.ptable_lock);

    new_process((u64) shell, "shell", 0, NULL);

    core_execute(1, smp_scheduler);
    core_execute(2, smp_scheduler);
    core_execute(3, smp_scheduler);
    smp_scheduler();   
}
