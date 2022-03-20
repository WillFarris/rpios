#include "shell.h"
#include "types.h"
#include "math.h"
#include "font.h"
#include "string.h"
#include "fb.h"
#include "printf.h"
#include "mini_uart.h"
#include "schedule.h"
#include "utils.h"

#define print_console printf
#define print_console_c uart_putc

extern u64 scheduler_ticks_per_second;

void test_print_and_exit(u64 a)
{
    print_console("\nTest process message 1\n> ");
    exit();
    print_console("\nTest process message 2 (should never be printed)\n> ");
}

void test_loop()
{
    int i=0;
    while(1) {
        printf("\niteration %d\n> ", i++);
        sys_timer_sleep_ms(2000);
    }
}

void exec(char **args)
{
    if(strcmp(args[0], "gcd") == 0)
    {
        u64 a = strtol(args[1]);
        u64 b = strtol(args[2]);
        print_console("%d\n> ", gcd(a, b));
    } else if(strcmp(args[0], "phi") == 0)
    {
        u64 n = strtol(args[1]);
        print_console("%d\n> ", phi(n));
    } else if(strcmp(args[0], "lcm") == 0)
    {
        u64 n = strtol(args[1]);
        u64 m = strtol(args[2]);
        print_console("%d\n> ", lcm(n, m));
    } else if(strcmp(args[0], "ord") == 0)
    {
        u64 a = strtol(args[1]);
        u64 m = strtol(args[2]);
        print_console("%d\n> ", ord(a, m));
    } else if (strcmp(args[0], "primefactors") == 0)
    {
        u64 n = strtol(args[1]);
        prime_factors(n);
        print_console("\n> ");
    } else if(strcmp(args[0], "clear") == 0)
    {
        new_process((u64) fbclear, 0, "clear screen");
    } else if(strcmp(args[0], "mod") == 0)
    {
        u64 a = strtol(args[1]);
        u64 b = strtol(args[2]);
        print_console("%d\n> ", a % b);
    }  else if(strcmp(args[0], "setres") == 0)
    {
        u64 w = strtol(args[1]);
        u64 h = strtol(args[2]);
        fbinit(w, h);
    } else if(strcmp(args[0], "ptable") == 0)
    {
        print_ptable();
    } else if(strcmp(args[0], "set_sched_tps") == 0)
    {
        scheduler_ticks_per_second = strtol(args[1]);
    } else if(strcmp(args[0], "kill") == 0)
    {
        kill(strtol(args[1]));
    } else if(strcmp(args[0], "test_loop") == 0)
    {
        new_process((u64) test_loop, 0, "print_loop");
    } else if(strcmp(args[0], "print_test") == 0)
    {
       new_process((u64) test_print_and_exit, 0, "test_process"); 
    } else if(strcmp(args[0], "help") == 0)
    {
        print_console("Available commands:\n"
                      "    gcd <a> <b>\n"
                      "    phi <n>\n"
                      "    lcm <a> <b>\n"
                      "    ord <a> <m>\n"
                      "    primefactors <n>\n"
                      "    clear\n"
                      "    mod <n> <m>\n"
                      "    setres <w> <h>\n"
                      "    ptable\n"
                      "    set_sched_tps <scheduler ticks per second>\n"
                      "    kill <pid>\n"
                      "    print_test\n"
                      "    test_loop\n"
                      ""
                      ""
                      "    help\n\n> "
                      );
    } else
    {
        print_console("Unknown command: %s\n> ", args[0]);
    }
}

void parse_command(char *str, char **args)
{
    int i=0;
    char *argstart = str;

    char * cur = str;
    while(*cur)
    {
        if(i > 9) return;
        if(*cur == ' ')
        {
            args[i++] = argstart;
            *cur = 0;
            argstart = ++cur;
        } else ++cur;
    }
    args[i++] = argstart;
    args[i] = 0;
    exec(args);
}



void shell()
{
    char commandbuffer[DISPLAY_WIDTH];
    char * args[10];
    char *cur = commandbuffer;
    u8 core = get_core();

    print_console_c('>');
    print_console_c(' ');
    while(1)
    {
        char c = uart_getc();
        switch(c)
        {
            case 0x7F:
                fb.cursor_x -= char_width;
                *(--cur) = 0;
                --cur;
                print_console_c(' ');
                fb.cursor_x -= char_width;
                break;
            case '\r':
                print_console("\n\r> ");
                *cur = 0;
                cur = commandbuffer;
                parse_command(commandbuffer, args);
                break;
            default:
                print_console_c(c);
                *cur = c;
                break;
        }
        if(c != '\r') ++cur;
    }
}
