#include "mini_uart.h"
#include "regstruct.h"

#define TXD 14
#define RXD 15

void uart_init()
{
    gpio_pin_set_func(14, GFAlt4);
    gpio_pin_set_func(15, GFAlt5);

    gpio_pin_enable(14);
    gpio_pin_enable(15);

    REGS_AUX->enables = 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 0xD;
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;

    REGS_AUX->mu_baud_rate = 270; // 115200 @ 250MHz

    REGS_AUX->mu_control = 3;

}
void uart_puts(const char * s)
{
    while(s && *s)
    {
        uart_putc(0, *s);
        ++s;
    }
}

void uart_putc(void *p, const char c)
{
    while(!(REGS_AUX->mu_lsr & 0x20));
    REGS_AUX->mu_io = c;
}

char uart_getc()
{
    while(!(REGS_AUX->mu_lsr & 1));
    return REGS_AUX->mu_io & 0xFF;
}