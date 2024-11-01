#include <common/rc.h>
#include <core/console.h>

void init_rc(RefCount* rc) {
    rc->count = 0;
}

void increment_rc(RefCount* rc) {
    __atomic_fetch_add(&rc->count, 1, __ATOMIC_ACQ_REL);
    // printf("inc%d\n", rc->count);
}

bool decrement_rc(RefCount* rc) {
    return __atomic_sub_fetch(&rc->count, 1, __ATOMIC_ACQ_REL) <= 0;
}
