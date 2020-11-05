#pragma once

#include "types.h"

#define CLOCKHZ 1000000

#define LOCAL_TIMER_RELOAD (0x249F000 << 2) // 38.4MHz / (38400000/4) = 0.25Hz

void sys_timer_init();
void local_timer_init();
void handle_sys_timer1_irq();
void handle_sys_timer3_irq();
void handle_local_timer_irq();

u64 sys_timer_get_ticks();
void sys_timer_sleep_ms();

enum timer_cs
{
    TIMER_CS_M0 = (1 << 0),
    TIMER_CS_M1 = (1 << 1),
    TIMER_CS_M2 = (1 << 2),
    TIMER_CS_M3 = (1 << 3),
};