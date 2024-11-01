#include <aarch64/mmu.h>
#include <common/types.h>
#pragma once

#ifndef _CORE_VIRTUAL_MEMORY_
#define _CORE_VIRTUAL_MEMORY_

#define USERTOP 0x0001000000000000
#define KERNBASE 0xFFFF000000000000

/*
 * uvm stands user vitual memory.
 */

typedef struct {
    PTEntriesPtr (*pgdir_init)(void);
    PTEntriesPtr (*pgdir_walk)(PTEntriesPtr pgdir,
                               void* kernel_address,
                               int alloc);
    void (*vm_free)(PTEntriesPtr pgdir);
    int (*uvm_map)(PTEntriesPtr pgdir,
                   void* kernel_address,
                   uint64_t size,
                   uint64_t physical_address);
} VMemory;

PTEntriesPtr pgdir_init(void);
PTEntriesPtr pgdir_walk(PTEntriesPtr pgdir, void* kernel_address, int alloc);
void vm_free(PTEntriesPtr pgdir);
int uvm_map(PTEntriesPtr pgdir,
            void* kernel_address,
            uint64_t size,
            uint64_t physical_address);
void uvm_switch(PTEntriesPtr pgdir);
void virtual_memory_init(VMemory*);
void init_virtual_memory();
void vm_test();

#define PGSIZE 4096  // bytes per page
#define PGSHIFT \
    12  // bits of offset within a page
        // shift a physical address to the right place for a PTE.
        // #define V2P(a) (((uint64_t)(a)) - KERNBASE)
        // #define P2V(a) ((void*)(((char*)(a)) + KERNBASE))

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK 0x1FF  // 9 bits
#define PXSHIFT(level) (PGSHIFT + (9 * (level)))
#define PX(level, va) ((((uint64_t)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (48 - 1))

#endif
