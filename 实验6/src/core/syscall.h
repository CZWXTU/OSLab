#pragma once

#include <core/trapframe.h>
#include <core/syscallno.h>

void sys_myexecve(char *s);
void sys_myexit();
void sys_myprint(int x);
u64 syscall_dispatch(Trapframe *frame);
