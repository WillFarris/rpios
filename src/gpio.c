#include "utils.h"
#include "regstruct.h"

void gpio_pin_set_func(u8 pin, GpioFunc func)
{
    u8 bit_start = (pin * 3) % 30;
    u8 reg = pin / 10;

    u32 selector = REGS_GPIO->func_select[reg];
    selector &= ~(7 << bit_start);
    selector |= (func << bit_start);

    REGS_GPIO->func_select[reg] = selector;
}

void gpio_pin_enable(u8 pin)
{
    REGS_GPIO->pupd_enable = 0;
    delay(150);
    REGS_GPIO->pupd_enable_clocks[pin / 32] = 1 << (pin % 32);
    delay(150);
    REGS_GPIO->pupd_enable = 0;
    REGS_GPIO->pupd_enable_clocks[pin / 32] = 0;
}