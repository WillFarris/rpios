#include "schedule.h"
#include "mm.h"
#include "printf.h"
#include "string.h"
#include "spinlock.h"
#include "font.h"

struct _ptable ptable;


void init_scheduler() {
    struct process *p = get_free_page();
    p->state = TASK_RUNNING;
    p->next = NULL;
    strcpy(p->name, "init");
    ptable.head = NULL;
    ptable.tail = NULL;
    ptable.lock = 0;
    for(int i=0;i<4;++i) {
        ptable.current[i] = NULL;
    }
    ptable.current[get_core()] = p;
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
    u8 core = get_core();
    ptable.current[core]->state = TASK_ZOMBIE;
    _schedule();
    irq_enable();
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
    
    u64 pid = 1 + ptable.num_procs++;
    p->pid = pid;

    p->next = NULL;
    if(!ptable.head) {
        ptable.head = p;
        ptable.tail = p;
    } else {
        ptable.tail->next = p;
        ptable.tail = p;
    }
    enable_preempt();

    return pid;
}

void _schedule() {
    irq_disable();
    u8 core = get_core();
    
    //uart_puts("acquiring lock\n");
    //__sync_fetch_and_add(&ptable.lock, 1);
    acquire(&ptable.lock);
    //while(__atomic_test_and_set(&ptable.lock, 1));
    //uart_puts("lock acquired\n");
    

    struct process *prev = ptable.current[core];
    struct process *next = ptable.head;

    if(!next) {
        //uart_puts("releasing lock\n");
        release(&ptable.lock);
        return;
    }
    
    if(prev->state == TASK_RUNNING) {
        ptable.tail->next = prev;
        prev->next = NULL;
        ptable.tail = prev;
    } else if(prev->state == TASK_ZOMBIE) {
        free_page(prev);
        prev = NULL;
    }
    ptable.head = next->next;
    if(!ptable.head) ptable.tail = NULL;
    
    next->next = NULL;
    ptable.current[core] = next;

    //uart_puts("releasing lock\n");
    release(&ptable.lock);

    if(prev)
        cpu_switch_to(prev, next);
    else
        cpu_ctx_restore(prev, next);
}


void schedule() {
    //u32 core = get_core();
    //if(ptable.current[core]) ptable.current[core]->counter = 0;
    _schedule();
}


void schedule_tail() {
    irq_enable();
	enable_preempt();
}

void print_ptable() {
    irq_disable();
    fbputc('\n');
    fbputc('\r');
    acquire(&ptable.lock);
    struct process *head = ptable.head;
    struct process *core = NULL;
    fbprintf("  cores:\n");
    for(int i=0;i<4;++i) {
        core = ptable.current[i];
        fbprintf("   [core %d] 0x%X %s\n", i, core ? core->pid : 0, core ? ptable.current[i]->name : "<null>");
    }
    fbprintf("\n  not running\n");
    while(head) {
        fbprintf("   [pid %d] %s\n", head->pid, head->name);
        head = head->next;
    }
    release(&ptable.lock);
    irq_enable();
}