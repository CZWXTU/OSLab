#include <aarch64/intrinsic.h>
#include <common/string.h>
#include <core/console.h>
#include <core/physical_memory.h>
#include <core/virtual_memory.h>
#include <core/sched.h>
#include <core/trap.h>
#include <driver/clock.h>
#include <driver/interrupt.h>
#include <core/proc.h>

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
	
    init_memory_manager();
    init_virtual_memory();
    init_sched();

    // vm_test();

    release_spinlock(&init_lock);
}

void hello() {
    printf("CPU %d: HELLO!\n", cpuid());
    reset_clock(1000);
}

void init_system_per_cpu() {
    init_clock();
    set_clock_handler(hello);
    init_trap();

	/* TODO: Lab5 uncomment to test interrupt */
    // test_kernel_interrupt();
    init_cpu(&simple_scheduler);
}

NORETURN void main() {
	/* TODO: Lab2 print */
	
    init_system_once();
    wait_spinlock(&init_lock);

    init_system_per_cpu();

    if (cpuid() == 0) {
        spawn_init_process();
        enter_scheduler();
    } else {
        enter_scheduler();
    }

}
