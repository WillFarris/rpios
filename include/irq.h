#pragma once

#include "gpio.h"
#include "types.h"

void irq_init_vectors();
void irq_enable();
void irq_disable();

void show_invalid_entry_message(u32 type, u64 esr, u64 address);
void enable_interrupt_controller();
void handle_irq();

enum vs_irqs
{
    TIMER_MATCH1 = (1 << 1),
    TIMER_MATCH3 = (1 << 3),
    AUX_IRQ = (1 << 29),
    UART_IRQ = (1 << 57),

};

struct irq_regs
{
    reg32 irq0_pending_0;
    reg32 irq0_pending_1;
    reg32 irq0_pending_2;
    reg32 fiq_control;
    reg32 irq0_enable_1;
    reg32 irq0_enable_2;
    reg32 irq0_enable_0;
    reg32 irq0_disable_1;
    reg32 irq0_disable_2;
    reg32 irq0_disable_0;
};

#define REGS_IRQ ((struct irq_regs *)(PBASE + 0x0000B200))

struct st_regs
{
    reg32 irq0_cs;
    reg32 irq0_clo;
    reg32 irq0_chi;
    reg32 irq0_c0;
    reg32 irq0_c1;
    reg32 irq0_c2;
    reg32 irq0_c3;
};

#define ST_REGS ((struct st_regs *)(PBASE + 0x00003000))