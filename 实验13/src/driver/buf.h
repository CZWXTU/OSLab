#pragma once

#include <common/defines.h>
#include <common/list.h>
#include <common/string.h>

#define BSIZE 512

#define B_VALID 0x2 /* Buffer has been read from disk. */
#define B_DIRTY 0x4 /* Buffer needs to be written to disk. */

struct buf {
    int flags;
    u32 blockno;
    u8 data[BSIZE];  // 1B*512

    /*
     * Add other necessary elements. It depends on you.
     */
    /* TODO: Lab7 driver. */
    struct buf *Prev, *Next;
};
struct bufQueue {
    struct buf *begin, *end;
    int sz;
};
void initBufQueue(struct bufQueue* x);
void push(struct bufQueue* x, struct buf* y);
void pop(struct bufQueue* x);
void size(struct bufQueue* x);
struct buf* front(struct bufQueue* x);
struct buf* back(struct bufQueue* x);
int empty(struct bufQueue* x);
/*
 * Add some useful functions to use your buffer list, such as push, pop and so
 * on.
 */

/* TODO: Lab7 driver. */