#include "types.h"
#include "mini_uart.h"
#include "printf.h"
#include "irq.h"


void kernel_main() 
{
    uart_init();
    init_printf(0, uart_putc);

    printf("\nRaspberry Pi 3 OS!\nWe are running in EL %d\n\n", get_el());

    irq_init_vectors();
    enable_interrupt_controller();
    irq_enable();

    while(1)
    {
        
    }
    
}