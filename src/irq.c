#include "irq.h"
#include "utils.h"
#include "types.h"
#include "printf.h"
#include "regstruct.h"
#include "mini_uart.h"
#include "timer.h"

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
    printf("ERROR CAUGHT: %s - %d, ESR: %X, Address: %X\n", entry_error_messages[type], esr, address);
}

void enable_interrupt_controller()
{
    // Enable Mini UART + Timer1 + Timer 3
    REGS_IRQ->irq0_enable_1 = AUX_IRQ | TIMER_MATCH1 | TIMER_MATCH3;
}

void handle_irq()
{
    u32 irq = REGS_IRQ->irq0_pending_1;
    while(irq)
    {
        // IRQ from UART
        if(irq & AUX_IRQ)
        {
            irq &= ~AUX_IRQ;
            while((REGS_AUX->mu_iir & 4) == 4)
            {
                char c = uart_getc();
                if(c == '\r')
                    uart_putc('\n');
                uart_putc(c);
            }
        }

        // This timer is not supported on QEMU
        if(irq & TIMER_MATCH1)
        {
            irq &= ~TIMER_MATCH1;
            handle_sys_timer1_irq();
        }

        // This timer is not supported on QEMU
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
    
}