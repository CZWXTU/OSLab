
#include <core/proc.h>
#include <aarch64/mmu.h>
#include <core/virtual_memory.h>
#include <core/physical_memory.h>
#include <common/string.h>
#include <core/sched.h>
#include <core/console.h>

void forkret();
extern void trap_return();
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
 * Step 7 (TODO): Set the context to work with `swtch()`, `forkret()` and `trap_return()`.
 */
static struct proc *alloc_proc() {

}

/*
 * Set up first user process(Only used once).
 * Step 1: Allocate a configured proc struct by `alloc_proc()`.
 * Step 2 (TODO): Allocate memory for storing the code of init process.
 * Step 3 (TODO): Copy the code (ranging icode to eicode) to memory.
 * Step 4 (TODO): Map any va to this page.
 * Step 5 (TODO): Set the address after eret to this va.
 * Step 6 (TODO): Set proc->sz.
 */
void spawn_init_process() {
    /* TODO: Lab6 Process */
}

/*
 * A fork child will first swtch here, and then "return" to user space.
 */
void forkret() {
	/* TODO: Lab6 Process */

}

/*
 * Exit the current process.  Does not return.
 * An exited process remains in the zombie state
 * until its parent calls wait() to find out it exited.
 */
NO_RETURN void exit() {
    struct proc *p = thiscpu()->proc;
    /* TODO: Lab6 Process */
	p->state = ZOMBIE;
    sched();

    PANIC("exit should not return\n");
}
void yield() {
    /* TODO: lab6 container */
    struct proc* p = thiscpu()->proc;
    p->state = RUNNABLE;
    sched();
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
        uvm_map(newpgdir, 0, PAGE_SIZE, K2P(newpage));

        p->tf->elr = 0;
        p->tf->spsr = 0;
        p->tf->sp = PAGE_SIZE;
        p->tf->x[30] = 0;

        p->state = RUNNABLE;
        p->sz = PAGE_SIZE;
    }
}
