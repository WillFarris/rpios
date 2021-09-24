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

enum pstate {
    TASK_SLEEP,
    TASK_RUNNING,
    TASK_ZOMBIE,
};

struct process {
    struct context ctx;
    enum pstate state;
    char name[20];
    u64 counter;
    u64 priority;
    u64 preempt;
    u64 pid;
    u8 core_in_use;
    struct process *next;
};

struct _ptable {
    u64 num_procs;
    struct process *head;
    struct process *tail;
    struct process * current[4];
    u8 lock;
} __attribute__ ((aligned (8)));

void disable_preempt();
void enable_preempt();

void init_scheduler();
void schedule();
i64 new_process(u64, u64, char*);
void ret_from_fork();
void print_ptable();
void exit();