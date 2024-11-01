#include <aarch64/intrinsic.h>
#include <common/string.h>
#include <core/virtual_memory.h>
#include <core/physical_memory.h>
#include <common/types.h>
#include <core/console.h>

/* For simplicity, we only support 4k pages in user pgdir. */

extern PTEntries kpgdir;
VMemory vmem;

PTEntriesPtr pgdir_init() {
    return vmem.pgdir_init();
}

PTEntriesPtr pgdir_walk(PTEntriesPtr pgdir, void *vak, int alloc) {
    
    return vmem.pgdir_walk(pgdir, vak, alloc);
}

void vm_free(PTEntriesPtr pgdir) {
    vmem.vm_free(pgdir);
}

int uvm_map(PTEntriesPtr pgdir, void *va, size_t sz, uint64_t pa) {
    return vmem.uvm_map(pgdir, va, sz, pa);
}

void uvm_switch(PTEntriesPtr pgdir) {
    arch_set_ttbr0(K2P(pgdir));
}


/*
 * generate a empty page as page directory
 */

static PTEntriesPtr my_pgdir_init() {
    /* TODO: Lab3/4 memory*/
}


/*
 * return the address of the pte in user page table
 * pgdir that corresponds to virtual address va.
 * if alloc != 0, create any required page table pages.
 */

static PTEntriesPtr my_pgdir_walk(PTEntriesPtr pgdir, void *vak, int alloc) {
    /* TODO: Lab3/4 memory*/
}


void my_vm_free(PTEntriesPtr pgdir) {
    /* TODO: Lab3/4 memory*/
}

/*
 * Create PTEs for virtual addresses starting at va that refer to
 * physical addresses starting at pa. va and size might not
 * be page-aligned.
 * Return -1 if failed else 0.
 */

int my_uvm_map(PTEntriesPtr pgdir, void *va, size_t sz, uint64_t pa) {
    /* TODO: Lab3/4 memory*/
}

void virtual_memory_init(VMemory *vmem_ptr) {
    vmem_ptr->pgdir_init = my_pgdir_init;
    vmem_ptr->pgdir_walk = my_pgdir_walk;
    vmem_ptr->vm_free = vm_free;
    vmem_ptr->uvm_map = my_uvm_map;
}

void init_virtual_memory() {
    virtual_memory_init(&vmem);
}

void vm_test() {
    /* TODO: Lab3/4 memory*/
	puts("In test");
    *((uint64_t *)P2K(0)) = 0xac;
    char *p = kalloc();

    memset(p, 0, PAGE_SIZE);

    vmem.uvm_map((uint64_t *)p, (void *)0x1000, PAGE_SIZE, 0);

    asm volatile("msr ttbr0_el1, %[x]" : : [x] "r"(K2P(p)));

    if (*((uint64_t *)0x1000) == 0xac) {
        puts("Test_Map_Region Pass!");
    } else {
        puts("Test_Map_Region Fail!\n");
    }
    // Certify that your code works!
}
