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
    u32 ptable_lock;
    u32 mem_map_lock;
    u32 counter_lock;
} locks;

void core_startup() {
    mmu_init();
    //print_sctlr();
    start_scheduler();
    //while(1) {}
}

void start_timed_scheduler() {
    irq_enable();
    core_timer_init();
    while(1) {
        //schedule();
    }
}

void core_draw_pi_logo() {
    draw_pi_logo(0, 0);
    exit();
}

void print_sctlr() {
    sys_timer_sleep_ms(100);
    printf("[core %d] sctlr_el1: 0x%X\n", get_core(), get_sctlr_el1());
}


extern void atomic_increment_asm();
void atomic_increment() {
    atomic_increment_asm(&locks.counter_lock);
}

void kernel_main() 
{
    uart_init_alt();    
    init_printf(0, putc);

    uart_puts("\n\nBooting Raspberry Pi 3\n\nBuilt "__TIME__" on "__DATE__"\n\n");

    locks.ptable_lock = 0;
    locks.mem_map_lock = 0;
    locks.counter_lock = 0;
    printf("[core %d] Initialized lock structure at address 0x%X\n", get_core(), &locks);

    //fbinit(1280, 720);

    init_page_tables(&locks);
    u8 ips = (u8)(get_id_aa64mmfr0_el1() & 0xF);
    print_pa_range_support(ips);
    mmu_init();
    irq_disable();

    QA7->control_register = 0b00 << 8;

    //init_ptable(&locks.ptable_lock);

    sys_timer_sleep_ms(100);

    core_execute(1, mmu_init);
    core_execute(2, mmu_init);
    core_execute(3, mmu_init);
    sys_timer_sleep_ms(200);

    printf("counter: %d\n", locks.counter_lock);
    core_execute(1, atomic_increment);
    sys_timer_sleep_ms(200);
    printf("counter: %d\n", locks.counter_lock);

    printf("CPUECTLR_EL1: 0x%x\n", get_cpuectlr_el1());

    while(1) {}
    
}
