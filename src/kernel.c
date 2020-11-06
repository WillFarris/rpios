#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"
#include "utils.h"
#include "timer.h"
#include "regstruct.h"


void kernel_main() 
{
    uart_init_alt();
    init_printf(0, putc);


    printf("\n\nBooting Raspberry Pi 3\n");
    
    printf("\nRunning on core %d\nWe are in EL%d\n\n", get_core(), get_el());

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();
    sys_timer_init();
    local_timer_init();

    printf("and... done!\n");
    
    while(1)
    {
        uart_putc('R');
        sys_timer_sleep_ms(100);
        uart_putc('u');
        sys_timer_sleep_ms(100);
        uart_putc('n');
        sys_timer_sleep_ms(100);
        uart_putc('n');
        sys_timer_sleep_ms(100);
        uart_putc('i');
        sys_timer_sleep_ms(100);
        uart_putc('n');
        sys_timer_sleep_ms(100);
        uart_putc('g');
        sys_timer_sleep_ms(100);
        uart_putc('.');
        sys_timer_sleep_ms(100);
        uart_putc('.');
        sys_timer_sleep_ms(100);
        uart_putc('.');
        sys_timer_sleep_ms(100);
        printf("\r           \r");
    }
    
}