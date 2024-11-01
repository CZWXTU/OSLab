#include <aarch64/mmu.h>
#include <common/spinlock.h>
#include <common/string.h>
#include <core/console.h>
#include <core/physical_memory.h>
#include <core/proc.h>
#include <core/sched.h>
#include <core/virtual_memory.h>
#include <driver/sd.h>
#include <fs/file.h>
#include <fs/fs.h>
#include <fs/inode.h>

void forkret();
extern void trap_return();
volatile int flag_atom = 0;

/*
 * Look through the process table for an UNUSED proc.
 * If found, change state to EMBRYO and initialize
 * state (allocate stack, clear trapframe, set context for switch...)
 * required to run in the kernel. Otherwise return 0.
 */
static struct proc *alloc_proc() {
    struct proc *p;
    /* TO-DO: Lab6 Process */
}

/*
 * Set up first user process(Only used once).
 * Set trapframe for the new process to run
 * from the beginning of the user process determined
 * by uvm_init
 */
void spawn_init_process() {
    struct proc* p;
    extern char icode[], eicode[];
    p = alloc_proc();

    /* TO-DO: Lab6 Process */
    release_sched_lock();
}

/*
 * A fork child will first swtch here, and then "return" to user space.
 */
void forkret() {
    release_sched_lock();
    if (!__atomic_test_and_set(&flag_atom, 1)) {
        init_filesystem();
        OpContext ctx;
        bcache.begin_op(&ctx);
        thiscpu()->proc->cwd = namei("/", &ctx);
        bcache.end_op(&ctx);
        // sd_test();
    }
}

/*
 * Exit the current process.  Does not return.
 * An exited process remains in the zombie state
 * until its parent calls wait() to find out it exited.
 */
void wakeup_withlock(void *chan);
NO_RETURN void exit() {
    struct proc *p = thiscpu()->proc;
    // if (p == initproc) {
    //     PANIC("exit: init process shall not exit!");
    // }

    for (int fd = 0; fd < NOFILE; fd++) {
        if (thiscpu()->proc->ofile[fd]) {
            fileclose(thiscpu()->proc->ofile[fd]);
            thiscpu()->proc->ofile[fd] = 0;
        }
    }
    OpContext ctx;
    bcache.begin_op(&ctx);
    inodes.put(&ctx, thiscpu()->proc->cwd);
    bcache.end_op(&ctx);

    thiscpu()->proc->cwd = 0;
    acquire_sched_lock();
    wakeup_withlock(p->parent);
    for (struct proc *p = thiscpu()->scheduler->ptable.proc;
         p < thiscpu()->scheduler->ptable.proc + NPROC;
         p++) {
        if (p->parent == thiscpu()->proc) {
            // p->parent = initproc;
            if (p->state == ZOMBIE) {
                wakeup_withlock(p->parent);
            }
        }
    }
    // release_sched_lock();
    p->state = ZOMBIE;
    sched();

    PANIC("exit should not return\n");
}

/*
 * Give up CPU.
 * Switch to the scheduler of this proc.
 */
void yield() {
    acquire_sched_lock();
    thiscpu()->proc->state = RUNNABLE;
    sched();
    release_sched_lock();
}

void sleep(void *chan, SpinLock *lock) {
    if (!holding_spinlock(lock)) {
        PANIC("sleep: lock not held");
    }

    // change the state of ptable, add lock
    if (lock != &thiscpu()->scheduler->ptable.lock) {
        acquire_sched_lock();
        release_spinlock(lock);
    }

    thiscpu()->proc->chan = chan;
    thiscpu()->proc->state = SLEEPING;
    sched();

    // sched returns
    thiscpu()->proc->chan = 0;

    if (lock != &thiscpu()->scheduler->ptable.lock) {
        release_sched_lock();
        acquire_spinlock(lock);
    }
}

void wakeup(void *chan) {
    acquire_sched_lock();
    for (struct proc *p = thiscpu()->scheduler->ptable.proc;
         p < thiscpu()->scheduler->ptable.proc + NPROC;
         p++) {
        if (p->state == SLEEPING && p->chan == chan) {
            p->state = RUNNABLE;
        }
    }
    release_sched_lock();
}

void wakeup_withlock(void *chan) {
    for (struct proc *p = thiscpu()->scheduler->ptable.proc;
         p < thiscpu()->scheduler->ptable.proc + NPROC;
         p++) {
        if (p->state == SLEEPING && p->chan == chan) {
            p->state = RUNNABLE;
        }
    }
}

void add_loop_test(int times) {
    for (int i = 0; i < times; i++) {
        struct proc *p;
        extern char loop_start[], loop_end[];
        p = alloc_proc();

        char *r = kalloc();
        if (r == NULL) {
            PANIC("uvm_init: cannot alloc a page");
        }
        memset(r, 0, PAGE_SIZE);
        uvm_map(p->pgdir, (void *)0, PAGE_SIZE, K2P(r));
        memmove(r, (void *)loop_start, (usize)(loop_end - loop_start));

        memset(p->tf, 0, sizeof(*(p->tf)));
        p->tf->spsr = 0;
        p->tf->sp = PAGE_SIZE;
        p->tf->x[30] = 0;
        p->tf->elr = 0;

        p->state = RUNNABLE;
    }
}

void idle_init() {
    struct proc *p;
    extern char ispin[], eicode[];
    p = alloc_proc();

    char *r = kalloc();
    if (r == NULL) {
        PANIC("uvm_init: cannot alloc a page");
    }
    memset(r, 0, PAGE_SIZE);
    uvm_map(p->pgdir, (void *)0, PAGE_SIZE, K2P(r));
    memmove(r, (void *)ispin, (usize)(eicode - ispin));

    memset(p->tf, 0, sizeof(*(p->tf)));
    p->tf->spsr = 0;
    p->tf->sp = PAGE_SIZE;
    p->tf->x[30] = 0;
    p->tf->elr = 0;
    p->sz = PAGE_SIZE;

    p->state = RUNNABLE;
}

int growproc(int n) {
    u32 sz;

    sz = (u32)thiscpu()->proc->sz;

    if (n > 0) {
        if ((sz = (u32)uvm_alloc(thiscpu()->proc->pgdir, 0, 0, sz, sz + (u32)n)) == 0) {
            return -1;
        }

    } else if (n < 0) {
        if ((sz = (u32)uvm_dealloc(thiscpu()->proc->pgdir, 0, sz, sz + (u32)n)) == 0) {
            return -1;
        }
    }

    thiscpu()->proc->sz = sz;
    uvm_switch(thiscpu()->proc->pgdir);

    return 0;
}

/*
 * Create a new process copying p as the parent.
 * Sets up stack to return as if from system call.
 * Caller must set state of returned proc to RUNNABLE.
 */
int fork() {
    //TODO Lab14 shell
}

/*
 * Wait for a child process to exit and return its pid.
 * Return -1 if this process has no children.
 */
int wait() {
    //TODO Lab14 shell
}
