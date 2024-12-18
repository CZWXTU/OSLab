#pragma once

#include <common/defines.h>

typedef struct {
    // read `BLOCK_SIZE` bytes in block at `block_no` to `buffer`.
    // caller must guarantee `buffer` is large enough.
    void (*read)(usize block_no, u8 *buffer);

    // write `BLOCK_SIZE` bytes from `buffer` to block at `block_no`.
    // caller must guarantee `buffer` contains at least `BLOCK_SIZE` bytes.
    void (*write)(usize block_no, u8 *buffer);
} BlockDevice;
