#include "gpio.h"
#include "regstruct.h"
#include "jtag.h"

#define GPFSEL0					(GPIO_ADDR + 0x00000000)
#define GPFSEL1					(GPIO_ADDR + 0x00000004)
#define GPFSEL2					(GPIO_ADDR + 0x00000008)
#define GPSET0					(GPIO_ADDR + 0x0000001C)
#define GPCLR0					(GPIO_ADDR + 0x00000028)
#define GPPUD					(GPIO_ADDR + 0x00000094)
#define GPPUDCLK0				(GPIO_ADDR + 0x00000098)
#define GPPUDCLK1				(GPIO_ADDR + 0x0000009C)

#define GPFSEL_PIN_MASK			(7U)//(BIT(2) | BIT(1) | BIT(0))
#define GPFSEL_ALT_4			(3U)//(BIT(1) | BIT(0))
#define GPFSEL_ALT_5			(2U)//(BIT(1))

void jtag_init() {
    u32 selector;
    
    selector = REGS_GPIO->func_select[2];
	selector &= ~(GPFSEL_PIN_MASK	<<  6);	// Gpio22 ->
	selector |=  (GPFSEL_ALT_4		<<  6);	// Alt4: ARM_TRST

	selector &= ~(GPFSEL_PIN_MASK	<<  9);	// Gpio23 -> 
	selector |=  (GPFSEL_ALT_4		<<  9);	// Alt4: ARM_RTCK
	
    selector &= ~(GPFSEL_PIN_MASK	<< 12);	// Gpio24 -> gpio9/pin21
	selector |=  (GPFSEL_ALT_4		<< 12);	// Alt4: ARM_TDO
	
    selector &= ~(GPFSEL_PIN_MASK	<< 15);	// Gpio25 -> gpio11/pin23
	selector |=  (GPFSEL_ALT_4		<< 15);	// Alt4: ARM_TCK
	
    selector &= ~(GPFSEL_PIN_MASK	<< 18);	// Gpio26 -> gpio10/pin19
	selector |=  (GPFSEL_ALT_4		<< 18);	// Alt4: ARM_TDI
	
    selector &= ~(GPFSEL_PIN_MASK	<< 21);	// Gpio27 -> gpio25/pin22
	selector |=  (GPFSEL_ALT_4		<< 21);	// Alt4: ARM_TMS

    REGS_GPIO->func_select[2] = selector;

    gpio_pin_enable(22);
    gpio_pin_enable(23);
    gpio_pin_enable(24);
    gpio_pin_enable(25);
    gpio_pin_enable(26);
    gpio_pin_enable(27);

}