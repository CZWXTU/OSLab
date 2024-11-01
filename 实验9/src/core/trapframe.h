#pragma once

#ifndef _CORE_TRAPFRAME_H_
#define _CORE_TRAPFRAME_H_

#include <common/defines.h>

/*
 * Trapframe should contain register x0~x30,
 * elr_el1, spsr_el1 and  sp_el0.
 * Pay attention to the order of these registers
 * in your trapframe.
 */
typedef struct {
	/* TODO: Lab5 Interrupt. */

    // TODO: this is a dirty hack, since musl's `memset` requires only SIMD register `q0`.
    u64 q0[2];
} Trapframe;

#endif
