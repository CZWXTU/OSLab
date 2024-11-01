#include <elf.h>

#include <aarch64/mmu.h>
#include <common/string.h>
#include <core/console.h>
#include <core/proc.h>
#include <core/sched.h>
#include <core/syscall.h>
#include <core/trap.h>
#include <core/virtual_memory.h>
#include <fs/file.h>
#include <fs/inode.h>

static uint64_t auxv[][2] = {{AT_PAGESZ, PAGE_SIZE}};

int execve(const char *path, char *const argv[], char *const envp[]) {
    //TODO Lab14 shell
}
