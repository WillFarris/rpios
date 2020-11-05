#include "timer.h"
#include "types.h"
#include "regstruct.h"
#include "utils.h"
#include "printf.h"

u32 cur_val_1 = 0;
u32 cur_val_3 = 0;

void timer_init()
{
   cur_val_1 = TIMER_REGS->timer_clo;
   cur_val_1 += CLOCKHZ;
   TIMER_REGS->timer_c1 = cur_val_1;
   cur_val_3 = TIMER_REGS->timer_clo;
   cur_val_3 += CLOCKHZ;TIMER_REGS->timer_c3 = cur_val_3;
}

void handle_timer_irq()
{
    printf("TIMER ");
    cur_val_1 += CLOCKHZ;
    TIMER_REGS->timer_c1 = cur_val_1;
    TIMER_REGS->timer_cs |= TIMER_CS_M1;
    printf("INTERRUPT!\n");
}