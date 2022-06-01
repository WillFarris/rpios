#include "schedule.h"
#include "mm.h"
#include "printf.h"
#include "string.h"
#include "spinlock.h"

struct _ptable ptable;

void init_ptable(u64 * lock_addr) {
    ptable.head = NULL;
    ptable.tail = NULL;
    ptable.lock = lock_addr;
    ptable.num_procs = 0;
    for(int i=0;i<4;++i) {
        ptable.current[i] = NULL;
    }
}

static u64 cnt = 0;

void start_scheduler() {
    irq_disable();
    acquire(ptable.lock);
    u8 core = get_core();
    struct process *p = get_free_page();
    p->state = TASK_RUNNING;
    p->next = NULL;
    u64 pid = 1 + ptable.num_procs++;
    p->pid = pid;
    strcpy(p->name, "kthread_");
    p->name[8] = core + '0';
    p->name[9] = '\0';
    ptable.current[core] = p;
    release(ptable.lock);

    core_timer_init();
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
    acquire(ptable.lock);
    u8 core = get_core();
    ptable.current[core]->state = TASK_ZOMBIE;
    release(ptable.lock);
    irq_enable();
    schedule();
}

i64 new_process(u64 entry, char*name, u64 argc, char**argv) {
    struct process *p = (struct process *) get_free_page();
    if(!p) return 0;

    strcpy(p->name, name);
    p->priority = 1;
    p->state = TASK_RUNNING;
    p->counter = p->priority;
    p->preempt = 1;
    p->ctx.x19 = entry;
    p->ctx.x20 = argc;
    p->ctx.x21 = argv;
    p->ctx.pc = (u64) ret_from_fork;
    p->ctx.sp = (u64) p + 4096;
    p->next = NULL;

    acquire(ptable.lock);
    u64 pid = 1 + ptable.num_procs++;
    p->pid = pid;
    if(!ptable.head) {
        ptable.head = p;
        ptable.tail = p;
    } else {
        ptable.tail->next = p;
        ptable.tail = p;
    }

    release(ptable.lock);

    return pid;
}

u64 get_pid() {
    irq_disable();
    acquire(ptable.lock);
    u64 pid = ptable.current[get_core()]->pid;
    release(ptable.lock);
    irq_enable();
    return pid;
}

void schedule() {
    u8 core = get_core();

    // Disable interrupts and lock ptable mutex
    irq_disable();
    acquire(ptable.lock);

    // Filter out any terminated processes
    while(ptable.head && ptable.head->state == TASK_ZOMBIE) {
        struct process *temp = ptable.head;
        ptable.head = ptable.head->next;
        free_page(temp);
    }

    // Determine next process to run from the list.
    // Return if there is nothing to switch to
    if(!ptable.head) {
        ptable.tail = NULL;
        release(ptable.lock);
        irq_enable();
        return;
    }
    struct process *next = ptable.head;

    // Pull the "next" task from the list
    ptable.head = ptable.head->next;
    if(!ptable.head) ptable.tail = NULL;

    // Move the current task from the list of currently running tasks
    // to the end of the task list
    struct process *prev = ptable.current[core];
    prev->next = NULL;
    ptable.current[core] = NULL;
    if(ptable.tail) {
        ptable.tail->next = prev;
        ptable.tail = prev;
    } else {
        ptable.head = prev;
        ptable.tail = prev;
    }

    ptable.current[core] = next;
    struct process * c = ptable.head;
    while(c) {
        c = c->next;
    }
    cpu_switch_to(prev, next);
    release(ptable.lock);
}

void schedule_tail() {
    __asm volatile ("dsb sy");
    release(ptable.lock);
    irq_enable();
    enable_preempt();
}

void kill(u64 argc, char**argv) {
    if(argc < 2) return;
    u64 pid = strtol(argv[1]);

    irq_disable();
    acquire(ptable.lock);
    
    for(int i=0;i<4;++i) {
        struct process *cur = ptable.current[i];
        if(cur && cur->pid == pid) {
            cur->state = TASK_ZOMBIE;
            release(ptable.lock);
            return;
        }
    }

    struct process *cur = ptable.head;
    while(cur) {
        if(cur->pid == pid) {
            cur->state = TASK_ZOMBIE;
            release(ptable.lock);
            return;
        }
        cur = cur->next;
    }

    release(ptable.lock);
    irq_enable();
}

#define print_console printf

void print_ptable() {
    __asm volatile ("dsb sy; isb sy");
    irq_disable();
    acquire(ptable.lock);
    
    u8 core = get_core();
    u64 lock_val = *ptable.lock;
    printf("[core %d] core vs. lock is %d vs %d, state is %s", core, core + 1, lock_val, lock_val > 0 ? "LOCKED" : "UNLOCKED");
    
    print_console("\nCPUs:\n");
    struct process *head = ptable.head;
    struct process *cur_core_proc = NULL;
    for(int i=0;i<4;++i) {
        cur_core_proc = ptable.current[i];
        print_console(
            " [core %d] pid %d, page 0x%X, %s\n", 
            i,
            cur_core_proc ? cur_core_proc->pid : 0,
            cur_core_proc,
            cur_core_proc ? cur_core_proc->name : "<null>"
        );
    }
    print_console("\nNot Running\n");
    while(head) {
        print_console(
            "  pid %d, page 0x%X, %s\n",
            head->pid,
            head,
            head->name
        );
        head = head->next;
    }
    print_console("\n");
    release(ptable.lock);
    irq_enable();
}
