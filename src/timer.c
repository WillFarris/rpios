#include "timer.h"
#include "types.h"
#include "regstruct.h"
#include "utils.h"
#include "schedule.h"
#include "printf.h"
#include "irq.h"
#include "font.h"

u32 cur_val_1 = 0;
u32 cur_val_3 = 0;
u64 scheduler_ticks_per_second = 100;

void core_timer_init() {
    u8 core = get_core();

    u64 freq = get_cntfrq_el0();
    u64 timer = freq / scheduler_ticks_per_second;
    write_cntp_tval(timer);
    enable_cntp();
    QA7->core_timer_interrupt_control[core] = 0xF;
}

void core_timer_handle_irq() {
    irq_disable();
    u64 freq = get_cntfrq_el0();
    u64 timer = freq / scheduler_ticks_per_second;
    write_cntp_tval(timer);
    
    schedule();
}

// Timer 1 will go off every 1 second
// Timer 5 will go off every 5 seconds
void sys_timer_init() {
    cur_val_1 = SYS_TIMER_REGS->timer_clo;
    cur_val_1 += CLOCKHZ;
    SYS_TIMER_REGS->timer_c1 = cur_val_1;

    /*cur_val_3 = SYS_TIMER_REGS->timer_clo;
    cur_val_3 += CLOCKHZ * 5;
    SYS_TIMER_REGS->timer_c3 = cur_val_3;*/
}

void handle_sys_timer1_irq() {
    cur_val_1 += CLOCKHZ;
    SYS_TIMER_REGS->timer_c1 = cur_val_1;
    SYS_TIMER_REGS->timer_cs |= TIMER_CS_M1;
    printf("System timer 1 Interrupt handled (%d) on core %d\n", cur_val_1, get_core());
}

void handle_sys_timer3_irq() {
    cur_val_3 += CLOCKHZ * 5;
    SYS_TIMER_REGS->timer_c3 = cur_val_3;
    SYS_TIMER_REGS->timer_cs |= TIMER_CS_M3;
    printf("System timer 3 Interrupt (every 5s)\n");
}

void local_timer_init(reg32 core, u8 fiq)
{
    LOCAL_TIMER->timer_control_core0 = 0xF;
    LOCAL_TIMER->timer_control_core1 = 0xF;
    LOCAL_TIMER->timer_control_core2 = 0xF;
    LOCAL_TIMER->timer_control_core3 = 0xF;
    LOCAL_TIMER->routing |= core + (fiq * 4);
    LOCAL_TIMER->control_status = (1 << 29) | (1 << 28) | LOCAL_TIMER_RELOAD;
    LOCAL_TIMER->set_clear_reload = (0x3 << 30);
}

void handle_local_timer_irq()
{
    //printf("\nLocal timer interrupt on core %d\n", get_core());
    LOCAL_TIMER->set_clear_reload = (0x3 << 30);
    schedule();
}

void sys_timer_sleep_ms(u64 ms)
{
    u64 start = sys_timer_get_ticks();
    while(sys_timer_get_ticks() < start + (ms * 1000));
}
