#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "timer.h"
#include "regstruct.h"


void kernel_main() 
{
    uart_init();
    init_printf(0, uart_putc);

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();
    timer_init();

    
#define pause 50000000
    while(1)
    {
        /*printf("System timer is %d\nTimer 1 compare is %d\nTimer 3 compare is %d\n",
            TIMER_REGS->timer_clo, TIMER_REGS->timer_c1, TIMER_REGS->timer_c3);
        delay(pause);*/
    }


    /*uart_putc(0, '\r');
    while(1)
    {
        uart_putc(0, 'R');
        delay(pause);
        uart_putc(0, 'u');
        delay(pause);

        uart_putc(0, 'n');
        delay(pause);
        uart_putc(0, 'n');
        delay(pause);

        uart_putc(0, 'i');
        delay(pause);
        uart_putc(0, 'n');
        delay(pause);

        uart_putc(0, 'g');
        delay(pause);
        uart_putc(0, '\r');
        delay(pause);
        printf("       ");
        uart_putc(0, '\r');

    }*/
    
}