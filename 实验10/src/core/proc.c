
#include <aarch64/mmu.h>
#include <common/string.h>
#include <core/console.h>
#include <core/physical_memory.h>
#include <core/proc.h>
#include <core/sched.h>
#include <core/virtual_memory.h>
#include <driver/sd.h>

void forkret();
extern void trap_return();
extern void initenter();
/*
 * Look through the process table for an UNUSED proc.
 * If found, change state to EMBRYO and initialize
 * state (allocate stack, clear trapframe, set context for switch...)
 * required to run in the kernel. Otherwise return 0.
 * Step 1 (TODO): Call `alloc_pcb()` to get a pcb.
 * Step 2 (TODO): Set the state to `EMBRYO`.
 * Step 3 (TODO): Allocate memory for the kernel stack of the process.
 * Step 4 (TODO): Reserve regions for trapframe and context in the kernel stack.
 * Step 5 (TODO): Set p->tf and p->context to the start of these regions.
 * Step 6 (TODO): Clear trapframe.
 * Step 7 (TODO): Set the context to work with `swtch()`, `forkret()` and
 * `trap_return()`.
 */
static struct proc* alloc_proc() {
    struct proc* p;
    /* TO-DO: Lab6 Process */
}

/*
 * Set up first user process(Only used once).
 * Set trapframe for the new process to run
 * from the beginning of the user process determined
 * by uvm_init
 * Step 1: Allocate a configured proc struct by `alloc_proc()`.
 * Step 2 (TODO): Allocate memory for storing the code of init process.
 * Step 3 (TODO): Copy the code (ranging icode to eicode) to memory.
 * Step 4 (TODO): Map any va to this page.
 * Step 5 (TODO): Set the address after eret to this va.
 * Step 6 (TODO): Set proc->sz.
 */
void spawn_init_process() {
    struct proc* p;
    extern char icode[], eicode[];
    p = alloc_proc();

    /* TO-DO: Lab6 Process */
}
void spawn_init_process_sd() {
    struct proc* p;
    extern char ispin[], eicode[];
    p = alloc_proc();

    /* TO-DO: Lab3 Process */
    if (p == 0) {
        PANIC("failed alloc proc");
    }
    void* newpgdir = pgdir_init();
    if (newpgdir == 0) {
        PANIC("failed to alloc pgdir");
    }
    p->pgdir = newpgdir;
    void* newpage = kalloc();
    memcpy(newpage, ispin, eicode - ispin);
    strncpy(p->name, "sdproc", sizeof(p->name));
    uvm_map(newpgdir, 0, PGSIZE, K2P(newpage));

    p->tf->elr_el1 = 0;
    p->tf->spsr_el1 = 0;
    p->tf->sp_el0 = PGSIZE;
    p->tf->x30 = 0;

    p->state = RUNNABLE;
    p->sz = PGSIZE;
}

/*
 * A fork child will first swtch here, and then "return" to user space.
 */
int sdtest = 0;
void forkret() {
    /* TO-DO: Lab3 Process */
    release_sched_lock();
    /* TO-DO: Lab3 Process */
    if (sdtest) {
        sd_test();
    }
    sdtest = 1;
    return;
}

/*
 * Exit the current process.  Does not return.
 * An exited process remains in the zombie state
 * until its parent calls wait() to find out it exited.
 */
NO_RETURN void exit() {
    /* TO-DO: Lab3 Process */
    acquire_sched_lock();
    struct proc* p = thiscpu()->proc;
    p->state = ZOMBIE;
    sched();
}

/*
 * Give up CPU.
 * Switch to the scheduler of this proc.
 */
void yield() {
    /* TODO: lab6 container */
    acquire_sched_lock();
    struct proc* p = thiscpu()->proc;
    p->state = RUNNABLE;
    sched();
    release_sched_lock();
}

/*
 * Atomically release lock and sleep on chan.
 * Reacquires lock when awakened.
 */
void sleep(void* chan, SpinLock* lock) {
    /* TODO: lab6 container */
    acquire_sched_lock();
    release_spinlock(lock);

    struct proc* p = thiscpu()->proc;
    p->chan = chan;
    p->state = SLEEPING;
    sched();
    p->chan = 0;
    release_sched_lock();
    acquire_spinlock(lock);
}

/* Wake up all processes sleeping on chan. */
void wakeup(void* chan) {
    /* TODO: lab6 container */
    acquire_sched_lock();
    struct proc* cp = thiscpu()->proc;
    struct proc* p;
    for (int i = 0; i < NPROC; i++) {
        p = &thiscpu()->scheduler->ptable.proc[i];
        if (p != cp && p->state == SLEEPING && p->chan == chan) {
            p->state = RUNNABLE;
        }
    }
    release_sched_lock();
}

/*
 * Add process at thiscpu()->container,
 * execute code in src/user/loop.S
 */
void add_loop_test(int times) {
    for (int i = 0; i < times; i++) {
        /* TODO: lab6 container */
        struct proc* p;
        extern char loop_start[], loop_end[];
        p = alloc_proc();
        if (p == 0) {
            PANIC("failed alloc proc");
        }
        void* newpgdir = pgdir_init();
        if (newpgdir == 0) {
            PANIC("failed to alloc pgdir");
        }
        p->pgdir = newpgdir;
        void* newpage = kalloc();
        memcpy(newpage, loop_start, loop_end - loop_start);
        strncpy(p->name, "initproc", sizeof(p->name));
        uvm_map(newpgdir, 0, PGSIZE, K2P(newpage));

        p->tf->elr_el1 = 0;
        p->tf->spsr_el1 = 0;
        p->tf->sp_el0 = PGSIZE;
        p->tf->x30 = 0;

        p->state = RUNNABLE;
        p->sz = PGSIZE;
    }
}
