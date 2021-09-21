#include "irq.h"
#include "utils.h"
#include "types.h"
#include "printf.h"
#include "regstruct.h"
#include "mini_uart.h"
#include "timer.h"
#include "font.h"

const char entry_error_messages[16][32] =
{
    "SYNC_INVALID_EL1t",
    "IRQ_INVALID_EL1t",
    "FIQ_INVALID_EL1t",
    "ERROR_INVALID_EL1t",

    "SYNC_INVALID_EL1h",
    "IRQ_INVALID_EL1h",
    "FIQ_INVALID_EL1h",
    "ERROR_INVALID_EL1h",

    "SYNC_INVALID_EL0_64",
    "IRQ_INVALID_EL0_64",
    "FIQ_INVALID_EL0_64",
    "ERROR_INVALID_EL0_64",

    "SYNC_INVALID_EL0_32",
    "IRQ_INVALID_EL0_32",
    "FIQ_INVALID_EL0_32",
    "ERROR_INVALID_EL0_32"
};

void show_invalid_entry_message(u32 type, u64 esr, u64 address)
{
    printf("ERROR CAUGHT: %s - %d, ESR: 0x%X, Address: 0x%X\n", entry_error_messages[type], esr, address);
}

void enable_interrupt_controller()
{
    // Timer1 + Timer 3 + Mini UART
    REGS_IRQ->irq0_enable_1 = TIMER_MATCH1 | TIMER_MATCH3 | AUX_IRQ;
}

u64 elapsed_ticks[4];

u32 irq_count = 0;
void handle_irq()
{
    //irq_disable();
    u32 irq = REGS_IRQ->irq0_pending_1;
    while(irq)
    {
        // IRQ from UART
        if(irq & AUX_IRQ)
        {
            irq &= ~AUX_IRQ;
            while((REGS_AUX->mu_iir & 4) == 4)
            {
                u8 c = uart_getc();
                fbputc(uart_getc());
                if(c == '\r') fbputc('\n');
                
                //shell();
            }
        }

        // This timer was previously not supported on QEMU
        if(irq & TIMER_MATCH1)
        {
            irq &= ~TIMER_MATCH1;
            handle_sys_timer1_irq();
        }

        // This timer was previously not supported on QEMU
        if(irq & TIMER_MATCH3)
        {
            irq &= ~TIMER_MATCH3;
            handle_sys_timer3_irq();
        }
    }

    u32 local_timer = LOCAL_TIMER->control_status;
    if(local_timer & (1 << 31))
    {
        // Local timer has run out
        handle_local_timer_irq();
    }

    u32 core = get_core();
    u32 core_irq_source = QA7->core_irq_source[core];
    if(core_irq_source & 2) { // & ((1 << 12)-1)
        core_timer_handle_irq();
    }
    //irq_enable();
}