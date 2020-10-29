#pragma once

void uart_init();
void uart_puts(const char*);
void uart_putc(void *, const char);
char uart_getc();