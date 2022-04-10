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

extern struct lock_table {
    u64 ptable_lock;
    u64 mem_map_lock;
    u64 test_lock;
} locks;

void core_startup() {
    mmu_init();
    print_sctlr();
    start_scheduler();
    //while(1) {}
}

void shell_core() {
    mmu_init();
    shell();
    while(1) {}
}

void start_timed_scheduler() {
    irq_enable();
    core_timer_init();
    while(1) {
        schedule();
    }
}

void core_draw_pi_logo() {
    draw_pi_logo(120 * get_core(), 0);
}

void print_sctlr() {
    sys_timer_sleep_ms(100);
    printf("[core %d] sctlr_el1: 0x%X\n", get_core(), get_sctlr_el1());
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    printf("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");

    locks.ptable_lock = 0;
    locks.mem_map_lock = 0;
    locks.test_lock = 0;

    init_page_tables(&locks);
    mmu_init();

    init_ptable(&locks.ptable_lock);
    QA7->control_register = 0b00 << 8;

    new_process((u64) test_loop, "test_loop", 0, NULL);

    core_execute(1, core_startup);
    sys_timer_sleep_ms(100);
    core_execute(2, core_startup);
    sys_timer_sleep_ms(100);

    core_execute(2, start_timed_scheduler);

    print_ptable();

    printf("[core %d] sctlr_el1: 0x%X\n", get_core(), get_sctlr_el1());
    shell();

    while(1) {}
}
