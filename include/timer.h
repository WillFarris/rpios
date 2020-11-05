#pragma once

#define CLOCKHZ 1000000

void timer_init();
void handle_timer_irq();

enum timer_cs
{
    TIMER_CS_M0 = (1 << 0),
    TIMER_CS_M1 = (1 << 1),
    TIMER_CS_M2 = (1 << 2),
    TIMER_CS_M3 = (1 << 3),
};