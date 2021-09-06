#include "schedule.h"
#include "mm.h"

struct _ptable ptable;

void init_scheduler() {
    ptable.num_procs = 0;
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
    disable_preempt();
    struct process *p = (struct process *) get_free_page();
    if(!p) return;

    p->priority = ptable.current->proc.priority;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt = 1;
    
    p->ctx.x19 = entry;
    p->ctx.x20 = arg;
    //p->ctx.pc = (u64) ret_from_fork;
    //p->ctx.sp = (u64) p + THREAD_SIZE;
    p->pid = ptable.num_procs++;
    enable_preempt();
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
