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


#define CMD_BUFFER_SIZE 128
#define MAX_SHELL_ARGS 16

extern u64 scheduler_ticks_per_second;

#define NUM_CMDS 2
struct command shell_cmds[NUM_CMDS] = {
    { "kill", "<pid>", kill },
    { "help", NULL, help }
};

#define NUM_PROGS 2
struct command shell_progs[NUM_PROGS] = {
    { "ptable", NULL, print_ptable },
    { "test_loop", "<ms delay>", test_loop }
};

void test_loop(int argc, char **argv) {
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
    printf("\nHere are the available programs:\n");
    for(int i=0;i<NUM_PROGS;++i) {
        printf("  %s %s\n", shell_progs[i].name, shell_progs[i].arghint == 0 ? "" : shell_progs[i].arghint);
    }
    printf("\n");
}

void parse_command(char * commandbuffer, char **args) {
    int argc=0;
    char *argstart = commandbuffer;

    char * cur = commandbuffer;
    while(*cur)
    {
        if(argc > MAX_SHELL_ARGS) return;
        if(*cur == ' ')
        {
            args[argc++] = argstart;
            *cur = 0;
            argstart = ++cur;
        } else ++cur;
    }
    args[argc++] = argstart;
    args[argc] = 0;
    
    /*printf("Parsed command with %d args\n[ ", argc);
    for(int i=0;i<argc;++i){
        printf("(%d, %s) ", i, args[i]);
    }
    printf("]\n");*/
    
    for(int n=0;n<NUM_CMDS;++n) {
        if(strcmp(args[0], shell_cmds[n].name) == 0)
        {
            void (*fun_ptr)(int, char**) =  shell_cmds[n].entry;
            fun_ptr(0, NULL);
        }
    }

    for(int n=0;n<NUM_PROGS;++n) {
        if(strcmp(args[0], shell_progs[n].name) == 0)
        {
            new_process((u64) shell_progs[n].entry, shell_progs[n].name, argc, args);
        }
    }
}

char commandbuffer[CMD_BUFFER_SIZE];
char args[MAX_SHELL_ARGS][CMD_BUFFER_SIZE];

void shell() {
    //char * commandbuffer = get_free_page();
    /*for(int i=0;i<CMD_BUFFER_SIZE;++i) {
        commandbuffer[i] = 0;
    }*/

    //char **args = commandbuffer+CMD_BUFFER_SIZE;
    /*for(int i=0;i<MAX_SHELL_ARGS;++i) {
        for(int j=0;j<CMD_BUFFER_SIZE;++j) {
            args[i][j] = 0;
        }
    }*/

    printf("Command buffer at 0x%x\n", commandbuffer, commandbuffer);
    printf("Argument array at 0x%x\n", args, args);

    u8 core = get_core();

    u32 ci = 0;
    printf("\nshell\n> ");
    while(1) {
        char c = uart_getc();
        switch(c) {
            case 0x7F:
                print_console("Got 0x7F (backspace)\n");
                /*fb.cursor_x -= char_width;
                *(--cur) = 0;
                --cur;
                print_console_c(' ');
                fb.cursor_x -= char_width;
                /break;*/
                break;
            case '\r':
                commandbuffer[ci] = 0;
                ci = 0;

                //uart_puts("\nParsing command buffer: [");
                //uart_puts(commandbuffer);
                uart_putc('\n');
                parse_command(commandbuffer, args);
                uart_puts("> ");
                break;
            default:
                print_console_c(c);
                commandbuffer[ci++] = c;
                break;
        }
        //if(c != '\r') ++cur;
    }
    free_page(commandbuffer);
    exit();
}