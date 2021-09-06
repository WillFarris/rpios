#include "schedule.h"
#include "mm.h"

void init_scheduler() {
    ptable.head = NULL;
    ptable.tail = NULL;
    ptable.current = NULL;
}

void disable_preempt() {
    if(ptable.current)
        ptable.current->proc.preempt += 1;
}

void enable_preempt() {
    if(ptable.current)
        ptable.current->proc.preempt -=1;
}

void new_process(u64 entry, u64 arg) {
    struct process *p = (struct process *) get_free_page();
    if(!p) return;

    p->priority = ptable.current->proc.priority;
    p->state = TASK_RUNNING;
    
    p->ctx.x19 = entry;
    p->ctx.x20 = arg;
    
}

void _schedule() {

    // Neither of these should happen
    if(!ptable.head) return;
    if(!ptable.current) return;

    disable_preempt();

    enable_preempt();
}

void schedule() {
    if(ptable.current) ptable.current->proc.counter = 0;
    _schedule();
}
