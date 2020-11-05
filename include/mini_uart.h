#pragma once

void uart_init();
void uart_puts(const char*);
void uart_putc(const char);
char uart_getc();
void putc(void *, const char);