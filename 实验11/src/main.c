#include <aarch64/intrinsic.h>
#include <common/string.h>
#include <core/arena.h>
#include <core/console.h>
#include <core/container.h>
#include <core/physical_memory.h>
#include <core/proc.h>
#include <core/sched.h>
#include <core/trap.h>
#include <core/virtual_memory.h>
#include <driver/clock.h>
#include <driver/interrupt.h>
#include <driver/sd.h>

struct cpu cpus[NCPU];

static SpinLock init_lock = {.locked = 0};

void init_system_once() {
    if (!try_acquire_spinlock(&init_lock))
        return;

    /* TODO: Lab2 set bss */
	extern char edata[], end[];

    init_interrupt();
    init_char_device();
    init_console();
    init_sched();

    init_memory_manager();
    init_virtual_memory();

    vm_test();
    arena_test();
    init_container();
    sd_init();

    release_spinlock(&init_lock);
}

void hello() {
    //printf("CPU %d: HELLO!\n", cpuid());
    reset_clock(1000);
    yield();
}

void init_system_per_cpu() {
    init_clock();
    set_clock_handler(hello);
    init_trap();

    /* TO-DO: Lab5 uncomment to test interrupt */
    // test_kernel_interrupt();
    init_cpu(&root_container->scheduler);
}

void main() {
    /* TO-DO: Lab2 print */

    init_system_once();
    wait_spinlock(&init_lock);

    init_system_per_cpu();
    /* TO-DO: Lab5 uncomment to test interrupt */
    // test_kernel_interrupt();
    if (cpuid() == 0) {
        spawn_init_process_sd();
        spawn_init_process_sd();
        // container_test_init();
        enter_scheduler();
    } else {
        enter_scheduler();
    }

    no_return();
}
