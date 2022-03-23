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

#define NUM_CMDS 4
struct command shell_cmds[NUM_CMDS] = {
    { "ptable", NULL, print_ptable },
    { "kill", "<pid>", kill },
    { "help", NULL, help },
    { "test_loop", "<ms delay>", test_loop }
};

void math(int argc, char **argv)
{
    if(argc < 4) {
        exit();
        return;
    }

    exit();
}

void test_loop(int argc, char **argv)
{
    int delay = 2000;
    if(argc > 1)
        delay = strtol(argv[1]);
    int i=0;
    u64 pid = get_pid();
    while(1) {
        u8 core = get_core();
        printf("\ncore %d pid %d iter %d\n> ", core, pid, i++);
        sys_timer_sleep_ms(delay);
    }
    exit();
}

void help(int argc, char **argv) {
    printf("Here are the available commands:\n");
    for(int i=0;i<NUM_CMDS;++i) {
        printf("  %s %s\n", shell_cmds[i].name, shell_cmds[i].arghint == 0 ? "" : shell_cmds[i].arghint);
    }
    printf("\n> ");
    exit();
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
    
    for(int n=0;n<NUM_CMDS;++n)
    {
        if(strcmp(args[0], shell_cmds[n].name) == 0)
        {
            new_process((u64) shell_cmds[n].entry, shell_cmds[n].name, i, args);
        }
    }
}



void shell()
{
    char commandbuffer[DISPLAY_WIDTH];
    char * args[10];
    char *cur = commandbuffer;
    u8 core = get_core();

    print_console("\nshell\n> ");
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
