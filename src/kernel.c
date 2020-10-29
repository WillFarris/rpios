#include "types.h"
#include "mini_uart.h"
#include "printf.h"

void kernel_main() 
{
    uart_init();
    init_printf(0, uart_putc);

    printf("\nRaspberry Pi 3 OS!\nWe are running in EL %d\n\n", get_el());

    while(1)
    {
        char c = uart_getc();
        if (c ==  '\r')
            uart_putc(0, '\n');
        uart_putc(0, c);
    }
    
}