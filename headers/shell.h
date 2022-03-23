#pragma once

#include "fb.h"

struct command {
    char *name;
    char *arghint;
    void *entry;
};

void exec(int, char **);
void help(int, char **);
void test_loop(int, char **);
void math(int, char **);
void shell();
void parse_command(char *, char **);
