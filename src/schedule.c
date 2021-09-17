#include "schedule.h"
#include "mm.h"
#include "printf.h"

struct _ptable ptable;


void init_scheduler() {
    struct process *p = get_free_page();
    p->state = TASK_RUNNING;
    ptable.head = p;
    ptable.tail = p;
}

void disable_preempt() {
    if(ptable.head)
        ptable.head->preempt += 1;
}

void enable_preempt() {
    if(ptable.head)
        ptable.head->preempt -=1;
}

void new_process(u64 entry, u64 arg) {

    if(!ptable.head || !ptable.tail) return;

    disable_preempt();
    struct process *p = (struct process *) get_free_page();
    if(!p) return;

    p->priority = 1;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt = 1;
    
    p->ctx.x19 = entry;
    p->ctx.x20 = arg;
    p->ctx.pc = (u64) ret_from_fork;
    p->ctx.sp = (u64) p + 4096;
    
    p->pid = 1 + ptable.num_procs++;

    p->next = ptable.head->next;
    if(!p->next) ptable.tail = p;
    ptable.head->next = p;
    enable_preempt();
}

void _schedule() {
    disable_preempt();

    printf("We are in the scheduler\n");

    struct process *prev;
    struct process *next;
    if(!ptable.head) return;
    else if(ptable.head == ptable.tail) return;
    else {
        prev = ptable.head;
        next = ptable.head->next;
        ptable.head = ptable.head->next;
        prev->next = NULL;
        ptable.tail->next = prev;
        ptable.tail = prev;
    }
    if(next == prev) return;
    cpu_switch_to(prev, next);
    enable_preempt();
}


void schedule() {
    if(ptable.head) ptable.head->counter = 0;
    _schedule();
}


void schedule_tail() {
	enable_preempt();
}