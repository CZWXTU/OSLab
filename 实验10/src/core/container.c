#include <common/string.h>
#include <core/arena.h>
#include <core/container.h>
#include <core/physical_memory.h>
#include <core/sched.h>
#include <core/virtual_memory.h>

struct container* root_container = 0;
static Arena arena;
bool do_cont_test = false;

extern void add_loop_test(int times);

/*
 * The entry of a new spawned scheduler.
 * Similar to `forkret`.
 * Maintain thiscpu()->scheduler.
 */
static NO_RETURN void container_entry() {
    /* TODO: lab6 container */
    release_sched_lock();
    thiscpu()->scheduler = &((container*)thiscpu()->proc->cont)->scheduler;
    if (do_cont_test)
        add_loop_test(2);
    enter_scheduler();
    /* container_entry should enter scheduler and should not return */
    PANIC("scheduler should not return");
}

/*
 * Allocate memory for a container.
 * For root container, a container scheduler is enough.
 * Memory of struct proc is from another ptable, if root is false.
 * Similar to `alloc_proc`.
 * Initialize some pointers.
 */
struct container* alloc_container(bool root) {
    /* TODO: lab6 container */

    container* c = alloc_object(&arena);
    init_spinlock(&c->lock, "container");
    c->scheduler.cont = c;
    if (root)
        return c;

    c->p = alloc_pcb();
    if (c->p == 0) {
        return 0;
    }
    c->p->state = EMBRYO;
    c->p->is_scheduler = true;
    c->p->cont = c;

    void* sp;
    for (int i = 0; i < NCPU; i++) {
        void* pg = kalloc();
        if (pg == 0) {
            PANIC("kalloc error");
        }
        sp = pg + PGSIZE - sizeof(struct context);
        c->scheduler.context[i] = sp;
        c->scheduler.context[i]->r30 = container_entry;
    }
    return c;
}

/*
 * Initialize the container system.
 * Initialize the memory pool and root scheduler.
 */
void init_container() {
    /* TODO: lab6 container */
    ArenaPageAllocator allocator = {.allocate = kalloc, .free = kfree};
    init_arena(&arena, sizeof(container), allocator);
    root_container = alloc_container(true);
    root_container->parent = root_container;
    root_container->scheduler.op = &simple_op;
    root_container->scheduler.parent = &(root_container->scheduler);
    root_container->scheduler.op->init(&root_container->scheduler);
}

/*
 * Allocating resource should be recorded by each ancestor of the process.
 * You can add parameters if needed.
 */
void* alloc_resource(struct container* this,
                     struct proc* p,
                     resource_t resource) {
    /* TODO: lab6 container */
    acquire_spinlock(&this->lock);
    switch (resource) {
        case PID:
            for (int i = 0; i < NPID; i++) {
                if (!this->pmap[i].valid) {
                    this->pmap[i].valid = true;
                    this->pmap[i].p = p;
                    this->pmap[i].pid_local = ++(this->scheduler.pid);
                    break;
                }
            }
            break;

        default:
            break;
    }
    if (this->parent != this) {
        alloc_resource(this->parent, p, PID);
    }
    release_spinlock(&this->lock);
    return this->scheduler.pid;
}

/*
 * Spawn a new process.
 */
struct container* spawn_container(struct container* this, struct sched_op* op) {
    /* TODO: lab6 container */
    container* c = alloc_container(false);
    if (c == 0) {
        PANIC("Failed alloc container");
    }
    c->parent = this;
    c->scheduler.cont = c;
    c->scheduler.op = op;
    c->scheduler.parent = &this->scheduler;
    c->scheduler.op->init(&c->scheduler);
    strncpy(c->p->name, "container", sizeof(c->p->name));
    c->p->parent = this->p;
    c->p->state = RUNNABLE;
}

/*
 * Add containers for test
 */
void container_test_init() {
    struct container* c;
    do_cont_test = true;
    add_loop_test(1);
    c = spawn_container(root_container, &simple_op);
    assert(c != NULL);
}
