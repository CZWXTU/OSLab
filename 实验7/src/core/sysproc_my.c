#include <core/syscall.h>
#include <core/console.h>
#include <core/proc.h>

void sys_myexecve(char *s) {
    printf("sys_exec: executing first program...\n");
    return;
}

NO_RETURN void sys_myexit() {
    printf("sys_exit: program in exit\n");
    exit();
}
