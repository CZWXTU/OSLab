#include <core/syscall.h>
#include <core/console.h>
#include <core/proc.h>

void sys_myexecve(char *s) {
    return;
}

NO_RETURN void sys_myexit() {
    exit();
}

void sys_myprint(int x) {
    printf("cnt %d\n", x);
    yield();
}
