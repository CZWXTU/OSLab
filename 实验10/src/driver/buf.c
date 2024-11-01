#include <common/defines.h>
#include <common/list.h>
#include <common/string.h>
#include <core/console.h>
#include <driver/buf.h>
void initBufQueue(struct bufQueue* x) {
    x->begin = x->end = NULL;
    x->sz = 0;
}
void push(struct bufQueue* x, struct buf* y) {
    if (x->sz == 0) {
        x->begin = x->end = y;
        x->sz = 1;
        return;
    }
    x->end->Next = y;
    y->Prev = x->end;
    x->end = y;
    x->sz++;
}
void pop(struct bufQueue* x) {
    if (x->sz == 0)
        PANIC("EMPTY");
    if (x->sz == 1) {
        x->begin = x->end = NULL;
        x->sz = 0;
        return;
    }
    x->begin = x->begin->Next;
    x->begin->Prev = NULL;
    x->sz--;
}
void size(struct bufQueue* x) {
    return x->sz;
}
struct buf* front(struct bufQueue* x) {
    if (x->sz == 0)
        PANIC("EMPTY");
    return x->begin;
}
struct buf* back(struct bufQueue* x) {
    if (x->sz == 0)
        PANIC("EMPTY");
    return x->end;
}
int empty(struct bufQueue* x) {
    return (x->sz == 0);
}