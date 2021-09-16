#include "tasks.h"
#include "utils.h"
#include "types.h"
#include "regstruct.h"

void print_uptime()
{
    while(1)
    {
        irq_disable();
        u32 core = get_core();

        u64 sys_timer = SYS_TIMER_REGS->timer_clo;      // Read low 32 bits
        sys_timer |= (SYS_TIMER_REGS->timer_chi << 32); // Read high 32 bits and combine
        sys_timer /= 1000;                              // Divide by 1000 to get value in ms
        
        u64 x = fb.cursor_x[core];
        u64 y = fb.cursor_y[core];
        fb.cursor_x[core] = 0;
        fb.cursor_y[core] = fb.height - (char_height * (core+1) * 2);
        fbprintf("Uptime: %dms", sys_timer);
        fb.cursor_x[core] = x;
        fb.cursor_y[core] = y;
        irq_enable();
    }
}