#include "schedule.h"
#include "mm.h"
#include "printf.h"
#include "string.h"
#include "spinlock.h"
#include "font.h"

struct _ptable ptable;


void flush_cache_process(u8 *p) {
    u64 proc_size_bytes = sizeof(struct process);
    //printf("Clearing cache for process at 0x%X (%d bytes)\n", p, proc_size_bytes);
    u8 *cur = p;
    while(cur < p + proc_size_bytes) {
        flush_cache(cur++);
    }
}

void flush_cache_ptable() {
    flush_cache(&ptable.head);
    flush_cache(&ptable.tail);
    //flush_cache(&ptable.lock); // Important not to flush this as apparently if messes with the exclusive access
    flush_cache(&ptable.num_procs);
    flush_cache(&ptable.current[0]);
    flush_cache(&ptable.current[1]);
    flush_cache(&ptable.current[2]);
    flush_cache(&ptable.current[3]);
}

void init_ptable() {
    ptable.head = NULL;
    ptable.tail = NULL;
    ptable.lock = 0;
    ptable.num_procs = 0;
    for(int i=0;i<4;++i) {
        ptable.current[i] = NULL;
    }
    flush_cache_ptable();
}

void start_scheduler() {
    irq_disable();
    acquire(&ptable.lock);
    u8 core = get_core();
    struct process *p = get_free_page();
    p->state = TASK_RUNNING;
    p->next = NULL;
    u64 pid = 1 + ptable.num_procs++;
    p->pid = pid;
    strcpy(p->name, "kernel_");
    p->name[7] = core + '0';
    p->name[8] = '\0';
    ptable.current[core] = p;

    flush_cache_process(p);
    flush_cache_ptable();
    
    printf("[core %d] created init task %s\n", core, p->name);
    release(&ptable.lock);
    irq_enable();

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
    flush_cache(&ptable.current[core]->state);
    release(&ptable.lock);
    irq_enable();
    schedule();
}

i64 new_process(u64 entry, char*name, u64 argc, char**argv) {

    __asm volatile ("dmb sy");
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

    acquire(&ptable.lock);
    flush_cache_ptable();
    u64 pid = 1 + ptable.num_procs++;
    p->pid = pid;
    if(!ptable.head) {
        ptable.head = p;
        ptable.tail = p;
    } else {
        ptable.tail->next = p;
        flush_cache(&ptable.tail->next);
        ptable.tail = p;
    }

    flush_cache_process(p);
    flush_cache_ptable();

    release(&ptable.lock);

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

#define DEBUG_CORE 4

void schedule() {
    u8 core = get_core();
    if(core == DEBUG_CORE)
        printf("[core %d] Running scheduler\n", core);

    // Disable interrupts and lock ptable mutex
    irq_disable();
    acquire(&ptable.lock);

    flush_cache_ptable();

    if(!ptable.head) {
        release(&ptable.lock);
        irq_enable();
        enable_preempt();
        return;
    }

    struct process *prev = ptable.current[core];
    
    // If the process currently on the CPU is still RUNNING,
    // append it to the process list
    if(prev && prev->state == TASK_RUNNING) {
        ptable.tail->next = prev;
        flush_cache(&ptable.tail);

        prev->core_in_use = 0xFF;
        prev->next = NULL;
        flush_cache_process(prev);

        ptable.tail = prev;
        flush_cache(&ptable.tail);
    }
    // Otherwise if the previous process is now a zombie, cleanup
    else if(prev && prev->state == TASK_ZOMBIE) {
        free_page(prev);
        prev = NULL;
        ptable.current[core] = NULL;
        flush_cache(&ptable.current[core]);
    }

    // Clean up any processes which were killed
    while (ptable.head && ptable.head->state == TASK_ZOMBIE) {
        free_page(ptable.head);
        ptable.head = ptable.head->next;
        flush_cache(&ptable.head);
    }

    // Next task to switch to is at the head of the list
    struct process *next = ptable.head;
    if(!next) { // Can't context switch if there's nothing to switch to
        release(&ptable.lock);
        return;
    }

    // We removed the head of the list, so update the rest of the list accordingly
    ptable.head = ptable.head->next;
    flush_cache(&ptable.head);
    if(!ptable.head) {
        ptable.tail = NULL;
        flush_cache(&ptable.tail);
    }

    // Set which processor is to be running the process in the ptable
    ptable.current[core] = next;
    flush_cache(&ptable.current[core]);

    next->core_in_use = core;
    next->next = NULL;
    flush_cache_process(next);

    cpu_switch_to(prev, next);
    release(&ptable.lock);
}

void schedule_tail() {
    release(&ptable.lock);
    irq_enable();
    enable_preempt();
}

void kill(u64 argc, char**argv) {
    if(argc < 2) return;
    u64 pid = strtol(argv[1]);

    irq_disable();
    acquire(&ptable.lock);
    for(int i=0;i<4;++i) {
        struct process *curproc = ptable.current[i];
        if(curproc && curproc->pid == pid) {
            curproc->state = TASK_ZOMBIE;
            flush_cache(&curproc->state);
            release(&ptable.lock);
            irq_enable();
            exit();
            return;
        }
    }
    struct process *cur = ptable.head;
    while(cur) {
        if(cur->pid == pid)  {
            cur->state = TASK_ZOMBIE;
            flush_cache(&cur->state);
            release(&ptable.lock);
            irq_enable();
            exit();
            return;
        }
        cur = cur->next;
    }
    release(&ptable.lock);
    irq_enable();
    exit();
}

#define print_console printf

void print_ptable() {
    irq_disable();
    acquire(&ptable.lock);
    print_console("\ncores:\n");
    struct process *head = ptable.head;
    struct process *cur_core_proc = NULL;
    for(int i=0;i<4;++i) {
        cur_core_proc = ptable.current[i];
        print_console(" [core %d] %d %s\n", i, cur_core_proc ? cur_core_proc->pid : 0, cur_core_proc ? cur_core_proc->name : "<null>");
    }
    print_console("\n  not running\n");
    while(head) {
        print_console(" [pid %d] %s\n", head->pid, head->name);
        head = head->next;
    }
    print_console("> ");
    release(&ptable.lock);
    irq_enable();
    exit();
}
