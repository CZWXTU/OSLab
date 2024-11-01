#include <aarch64/intrinsic.h>
#include <common/string.h>
#include <core/console.h>
#include <core/physical_memory.h>
#include <core/proc.h>
#include <core/sched.h>
#include <core/trap.h>
#include <core/virtual_memory.h>
#include <driver/clock.h>
#include <driver/interrupt.h>

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
    /* TODO: Lab2 set bss */
	extern char edata[], end[];

    init_interrupt();
    init_char_device();
    init_console();
    /* TO-DO: Lab2 print */
  //  printf("cpu%d: ",cpuid());
//    printf("Hello world!\n");
    init_memory_manager();
    init_virtual_memory();

    init_system_per_cpu();

    /* TODO: Lab5 uncomment to test interrupt */
    // test_kernel_interrupt();
    if (cpuid() == 0){
    	spawn_init_process();
        enter_scheduler();
        printf("cpu:%d:Hello, world!",cpuid());
    }
    else if (cpuid() == 1){
    //        puts("Hello, rpi-os!");
        printf("cpu:%d:Hello, world!",cpuid());    
    }
    else if (cpuid() == 2){
    //        puts("Hello, rpi-os!");
        printf("cpu:%d:Hello, world!",cpuid());    
    }
    else if (cpuid() == 3){
    //        puts("Hello, rpi-os!");
        printf("cpu:%d:Hello, world!",cpuid());    
    }
    else {
        enter_scheduler();
    }

    printf("Hello world end!\n");

    enter_scheduler();
    // vm_test();
    printf("bye~\n");
    /* TO-DO: Lab2 print */
}
