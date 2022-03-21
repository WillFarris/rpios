#include "schedule.h"
#include "mm.h"
#include "printf.h"
#include "string.h"
#include "spinlock.h"
#include "font.h"

struct _ptable ptable;

void init_ptable() {
    ptable.head = NULL;
    ptable.tail = NULL;
    ptable.lock = 0;
    for(int i=0;i<4;++i) {
        ptable.current[i] = NULL;
    }
}

void start_scheduler() {
    struct process *p = get_free_page();
    p->state = TASK_RUNNING;
    p->next = NULL;
    strcpy(p->name, "init ");
    p->name[5] = get_core() + '0';
    p->name[6] = '\0';
    ptable.current[get_core()] = p;

    core_timer_init();
    while(1) {
        schedule();
    }
}

void disable_preempt() {
    if(ptable.head)
        ptable.head->preempt += 1;
}

void enable_preempt() {
    if(ptable.head)
        ptable.head->preempt -=1;
}

void exit() {
    irq_disable();
    acquire(&ptable.lock);
    u8 core = get_core();
    ptable.current[core]->state = TASK_ZOMBIE;
    release(&ptable.lock);
    schedule();
}

i64 new_process(u64 entry, u64 arg, char*name) {

    disable_preempt();
    struct process *p = (struct process *) get_free_page();
    if(!p) return 0;

    strcpy(p->name, name);

    p->priority = 1;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt = 1;
    
    p->ctx.x19 = entry;
    p->ctx.x20 = arg;
    p->ctx.pc = (u64) ret_from_fork;
    p->ctx.sp = (u64) p + 4096;

    p->next = NULL;

    acquire(&ptable.lock);
    u64 pid = 1 + ptable.num_procs++;
    p->pid = pid;
    if(!ptable.head) {
        ptable.head = p;
        ptable.tail = p;
    } else {
        ptable.tail->next = p;
        ptable.tail = p;
    }
    release(&ptable.lock);
    enable_preempt();

    return pid;
}

u64 get_pid() {
    irq_disable();
    acquire(&ptable.lock);

    u64 pid = ptable.current[get_core()]->pid;

    release(&ptable.lock);
    irq_enable();

    return pid;
}

void schedule() {

    // Disable interrupts and lock ptable mutex
    irq_disable();
    acquire(&ptable.lock);
    u8 core = get_core();

    // Ger the process currently running on the CPU
    struct process *prev = ptable.current[core];
    
    // If the process currently on the CPU is still RUNNING,
    // append it to the process list
    if(prev && prev->state == TASK_RUNNING) {
        ptable.tail->next = prev;
        prev->core_in_use = 0xFF;
        prev->next = NULL;
        ptable.tail = prev;
    }
    // Otherwise if the previous process is now a zombie, cleanup
    else if(prev && prev->state == TASK_ZOMBIE) {
        free_page(prev);
        prev = NULL;
        ptable.current[core] = NULL;
        printf("\nFreed ded proc\n> ");
    }

    // Clean up any processes which were killed
    while (ptable.head && ptable.head->state == TASK_ZOMBIE) {
        free_page(ptable.head);
        ptable.head = ptable.head->next;
    }

    // Next task to switch to is at the head of the list
    struct process *next = ptable.head;
    if(!next) { // Can't context switch if there's nothing to switch to
        release(&ptable.lock);
        return;
    }

    // We removed the head of the list, so update the rest of the list accordingly
    ptable.head = ptable.head->next;
    if(!ptable.head) ptable.tail = NULL;

    // Set which processor is to be running the process in the ptable
    ptable.current[core] = next;
    ptable.current[core]->core_in_use = core;
    ptable.current[core]->next = NULL;

    // If there's a previous task still running, save it
    cpu_switch_to(prev, next);
    release(&ptable.lock);
}

void schedule_tail() {
    release(&ptable.lock);
    irq_enable();
    enable_preempt();
}

void kill(u64 pid) {
    irq_disable();
    acquire(&ptable.lock);
    for(int i=0;i<4;++i) {
        struct process *curproc = ptable.current[i];
        if(curproc && curproc->pid == pid) {
            curproc->state = TASK_ZOMBIE;
            release(&ptable.lock);
            irq_enable();
            return;
        }
    }
    struct process *cur = ptable.head;
    while(cur) {
        if(cur->pid == pid)  {
            cur->state = TASK_ZOMBIE;
            release(&ptable.lock);
            irq_enable();
            return;
        }
        cur = cur->next;
    }
    release(&ptable.lock);
    irq_enable();
}

#define print_console printf

void print_ptable() {
    irq_disable();
    print_console('\n');
    print_console('\r');
    acquire(&ptable.lock);
    struct process *head = ptable.head;
    struct process *core = NULL;
    print_console("  cores:\n");
    for(int i=0;i<4;++i) {
        core = ptable.current[i];
        print_console("   [core %d] 0x%X %s\n", i, core ? core->pid : 0, core ? ptable.current[i]->name : "<null>");
    }
    print_console("\n  not running\n");
    while(head) {
        print_console("   [pid %d] %s\n", head->pid, head->name);
        head = head->next;
    }
    print_console("> ");
    release(&ptable.lock);
    irq_enable();
}
