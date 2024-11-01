#include <common/defines.h>
#include <core/console.h>
#include <core/container.h>
#include <core/sched.h>
#include <core/virtual_memory.h>

#ifdef MULTI_SCHEDULER

/* TODO: Lab6 Add more Scheduler Policies */
static void scheduler_simple(struct scheduler* this);
static struct proc* alloc_pcb_simple(struct scheduler* this);
static void sched_simple(struct scheduler* this);
static void init_sched_simple(struct scheduler* this);
static void acquire_ptable_lock(struct scheduler* this);
static void release_ptable_lock(struct scheduler* this);
struct sched_op simple_op = {.scheduler = scheduler_simple,
                             .alloc_pcb = alloc_pcb_simple,
                             .sched = sched_simple,
                             .init = init_sched_simple,
                             .acquire_lock = acquire_ptable_lock,
                             .release_lock = release_ptable_lock};
struct scheduler simple_scheduler = {.op = &simple_op};

void swtch(struct context**, struct context*);

static void init_sched_simple(struct scheduler* this) {
    init_spinlock(&this->ptable.lock, "ptable");
}

static void acquire_ptable_lock(struct scheduler* this) {
    acquire_spinlock(&this->ptable.lock);
}

static void release_ptable_lock(struct scheduler* this) {
    release_spinlock(&this->ptable.lock);
}

/*
 * Scheduler yields to its parent scheduler.
 * If this == root, just return.
 * Pay attention to thiscpu() structure and locks.
 */
// TODO
void yield_scheduler(struct scheduler* this) {
    if (this == this->parent)
        return;

    acquire_ptable_lock(this->parent);
    thiscpu()->scheduler = this->parent;
    this->cont->p->state = RUNNABLE;
    release_ptable_lock(this);

    swtch(&(this->context[cpuid()]), this->parent->context[cpuid()]);

    acquire_ptable_lock(this);
    thiscpu()->scheduler = this;
    release_ptable_lock(this->parent);
}

NO_RETURN void scheduler_simple(struct scheduler* this) {
    proc* p;
    struct cpu* c = thiscpu();
    c->proc = this->cont->p;
    for (;;) {
        for (p = this->ptable.proc; p != (this->ptable.proc) + NPROC; ++p) {
            acquire_sched_lock();
            if (p->state == RUNNABLE) {
                p->state = RUNNING;
                c->proc = p;
                if (p->is_scheduler) {
                    // printf("scheduler_simple is_scheduler\n");
                    swtch(&(c->scheduler->context[cpuid()]),
                          ((container*)p->cont)->scheduler.context[cpuid()]);

                } else {
                    uvm_switch(p->pgdir);
                    // printf("scheduler_simple is_not_scheduler\n");
                    swtch(&(c->scheduler->context[cpuid()]), p->context);
                }

                c->proc = this->cont->p;
                yield_scheduler(this);
                c->proc = this->cont->p;
            }
            release_sched_lock();
        }
        acquire_sched_lock();
        yield_scheduler(this);
        release_sched_lock();
    }
}

static void sched_simple(struct scheduler* this) {
    if (!holding_spinlock(&this->ptable.lock)) {
        PANIC("sched: not holding ptable lock");
    }
    if (thiscpu()->proc->state == RUNNING) {
        PANIC("sched: process running");
    }
    struct proc* p = thiscpu()->proc;
    swtch(&p->context, thiscpu()->scheduler->context[cpuid()]);
}

static struct proc* alloc_pcb_simple(struct scheduler* this) {
    acquire_sched_lock();
    proc* p = this->ptable.proc;
    for (int i = 0; i < NPROC; i++) {
        if (p[i].state == UNUSED) {
            p[i].pid = (int)(alloc_resource(this->cont, &(p[i]), PID));
            release_sched_lock();
            return &(p[i]);
        }
    }
    release_sched_lock();
    return 0;
}

#endif
