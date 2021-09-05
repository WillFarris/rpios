#include "schedule.h"

struct _proc_list {
    struct pnode *head;
    struct pnode *tail;
    struct pnode *current;
} proc_list;

void init_scheduler() {
    proc_list.head = NULL;
    proc_list.tail = NULL;
    proc_list.current = NULL;
}