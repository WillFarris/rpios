#include "shell.h"
#include "types.h"
#include "math.h"
#include "font.h"
#include "string.h"
#include "fb.h"
#include "printf.h"

void exec(char **args)
{
    if(strcmp(args[0], "gcd") == 0)
    {
        u64 a = strtol(args[1]);
        u64 b = strtol(args[2]);
        fbprintf("%d\n> ", gcd(a, b));
    } else if(strcmp(args[0], "phi") == 0)
    {
        u64 n = strtol(args[1]);
        fbprintf("%d\n> ", phi(n));
    } else if (strcmp(args[0], "primefactors") == 0)
    {
        u64 n = strtol(args[1]);
        prime_factors(n);
        fbprintf("\n> ");
    } else if(strcmp(args[0], "clear") == 0)
    {
        fbclear(0x800000);
        fbprintf("> ");
    } else if(strcmp(args[0], "mod") == 0)
    {
        u64 a = strtol(args[1]);
        u64 b = strtol(args[2]);
        fbprintf("%d\n> ", a % b);
    }  else if(strcmp(args[0], "setres") == 0)
    {
        u64 w = strtol(args[1]);
        u64 h = strtol(args[2]);
        fbsetres(w, h);
    } else
    {
        fbprintf("Unknown command: %s\n> ", args[0]);
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

    fbprintf("\n> ");
    while(1)
    {
        char c = uart_getc();
        switch(c)
        {
            case 0x7F:
                fb.cursor_x -= char_width;
                *(--cur) = 0;
                --cur;
                fbputc(' ');
                fb.cursor_x -= char_width;
                break;
            case '\r':
                fbputc('\n');
                fbputc('\r');
                fbputc('>');
                fbputc(' ');
                *cur = 0;
                cur = commandbuffer;
                parse_command(commandbuffer, args);
                break;
            default:
                fbputc(c);
                *cur = c;
                break;
        }
        if(c != '\r') ++cur;
    }
}