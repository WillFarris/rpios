#include "mini_uart.h"
#include "regstruct.h"

#define TXD 14
#define RXD 15

#define GPFSEL1         (PBASE+0x00200004)

void uart_init_alt ( void )
{
	unsigned int selector;

	selector = get32(GPFSEL1);
	selector &= ~(7<<12);           // clean gpio14
	selector |= GFAlt5<<12;         // set alt5 for gpio14

	selector &= ~(7<<15);           // clean gpio15
	selector |= GFAlt5<<15;         // set alt5 for gpio15
	
    put32(GPFSEL1,selector);


    gpio_pin_enable(TXD);
    gpio_pin_enable(RXD);

	REGS_AUX->enables = 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 0xD;
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;
    REGS_AUX->mu_baud_rate = 270; // 115200 @ 250MHz
    REGS_AUX->mu_control = 3;
}

void uart_init()
{
    gpio_pin_set_func(14, GFAlt5);
    gpio_pin_set_func(15, GFAlt5);

    gpio_pin_enable(14);
    gpio_pin_enable(15);

    REGS_AUX->enables = 1;
    REGS_AUX->mu_control = 0;
    REGS_AUX->mu_ier = 0;
    REGS_AUX->mu_lcr = 3;
    REGS_AUX->mu_mcr = 0;
    REGS_AUX->mu_baud_rate = 270; // 115200 @ 250MHz
    REGS_AUX->mu_control = 3;

}
void uart_puts(const char * s)
{
    while(s && *s)
    {
        uart_putc(*s);
        ++s;
    }
}

void uart_putc(const char c)
{
    if(c == '\n')
        uart_putc('\r');
    while(!(REGS_AUX->mu_lsr & 0x20));
    REGS_AUX->mu_io = c;
}

char uart_getc()
{
    while(!(REGS_AUX->mu_lsr & 1));
    return REGS_AUX->mu_io & 0xFF;
}

void putc(void *p, char c)
{
    uart_putc(c);
}