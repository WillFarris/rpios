#pragma once

#include "types.h"

struct context {
    u64 x19;
    u64 x20;
    u64 x21;
    u64 x22;
    u64 x23;
    u64 x24;
    u64 x25;
    u64 x26;
    u64 x27;
    u64 x28;
    u64 fp;
    u64 sp;
    u64 pc;
};

struct process {
    struct context ctx;
    long state;
    long counter;
    long priority;
    long preempt;
};

struct pnode {
    struct process proc;
    struct task_node *next;
};

void init_scheduler();