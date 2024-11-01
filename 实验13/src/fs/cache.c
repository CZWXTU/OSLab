#include <common/bitmap.h>
#include <common/string.h>
#include <core/arena.h>
#include <core/console.h>
#include <core/physical_memory.h>
#include <core/proc.h>
#include <fs/cache.h>

static const SuperBlock* sblock;
static const BlockDevice* device;

static SpinLock lock;     // protects block cache.
static Arena arena;       // memory pool for `Block` struct.
static ListNode head;     // the list of all allocated in-memory block.
static LogHeader header;  // in-memory copy of log header block.

// hint: you may need some other variables. Just add them here.
struct LOG {
    /* data */
    SpinLock lock;
    int outstanding;
    int committing;
    int mu;
    int mx;
} log;

// read the content from disk.
static INLINE void device_read(Block* block) {
    device->read(block->block_no, block->data);
}

// write the content back to disk.
static INLINE void device_write(Block* block) {
    device->write(block->block_no, block->data);
}

// read log header from disk.
static INLINE void read_header() {
    device->read(sblock->log_start, (u8*)&header);
}

// write log header back to disk.
static INLINE void write_header() {
    device->write(sblock->log_start, (u8*)&header);
}

// initialize a block struct.
static void init_block(Block* block) {
    block->block_no = 0;
    init_list_node(&block->node);
    block->acquired = false;
    block->pinned = false;

    init_sleeplock(&block->lock, "block");
    // printf("%d\n", (void*)(block->data) - (void*)block);
    block->valid = false;
    memset(block->data, 0, sizeof(block->data));
}

// see `cache.h`.
static usize get_num_cached_blocks() {
    //TODO Lab13 cache
}

// see `cache.h`.
static Block* cache_acquire(usize block_no) {
    //TODO Lab13 cache
}

// see `cache.h`.
static void cache_release(Block* block) {
    //TODO Lab13 cache
}

void install_trans(int recovering) {
    for (u32 tail = 0; tail < header.num_blocks; tail++) {
        Block* lbuf = cache_acquire((usize)(sblock->log_start + tail + 1));
        Block* dbuf = cache_acquire((usize)(header.block_no[tail]));
        memmove(dbuf->data, lbuf->data, BLOCK_SIZE);
        device_write(dbuf);
        if (recovering)
            dbuf->pinned = 0;
        cache_release(lbuf);
        cache_release(dbuf);
    }
}

void recover_from_log() {
    read_header();
    install_trans(1);
    header.num_blocks = 0;
    write_header();
}

// initialize block cache.
void init_bcache(const SuperBlock* _sblock, const BlockDevice* _device) {
    sblock = _sblock;
    device = _device;

    //TODO Lab13 cache
}

// see `cache.h`.
static void cache_begin_op(OpContext* ctx) {
    //TODO Lab13 cache
}

// see `cache.h`.
static void cache_sync(OpContext* ctx, Block* block) {
    if (ctx) {
        //TODO Lab13 cache
    } else
        device_write(block);
}
void write_log() {
    for (u32 tail = 0; tail < header.num_blocks; tail++) {
        Block* from = cache_acquire(header.block_no[tail]);
        Block* to = cache_acquire(sblock->log_start + tail + 1);
        memmove(to->data, from->data, BLOCK_SIZE);
        device_write(to);
        cache_release(from);
        cache_release(to);
    }
}
void commit() {
    if (header.num_blocks > 0) {
        write_log();
        write_header();
        install_trans(0);
        header.num_blocks = 0;
        write_header();
    }
}
// see `cache.h`.
static void cache_end_op(OpContext* ctx) {
    //TODO Lab13 cache
}

// see `cache.h`.
// hint: you can use `cache_acquire`/`cache_sync` to read/write blocks.
usize BBLOCK(usize b, SuperBlock* sb) {
    return b / BIT_PER_BLOCK + sb->bitmap_start;
}
void bzero(OpContext* ctx, u32 block_no) {
    Block* bp = cache_acquire(block_no);
    memset(bp->data, 0, BLOCK_SIZE);
    cache_sync(ctx, bp);
    cache_release(bp);
}
static usize cache_alloc(OpContext* ctx) {
    //TODO Lab13 cache
    PANIC("cache_alloc: no free block");
}

// see `cache.h`.
// hint: you can use `cache_acquire`/`cache_sync` to read/write blocks.
static void cache_free(OpContext* ctx, usize block_no) {
    //TODO Lab13 cache
}

BlockCache bcache = {
    .get_num_cached_blocks = get_num_cached_blocks,
    .acquire = cache_acquire,
    .release = cache_release,
    .begin_op = cache_begin_op,
    .sync = cache_sync,
    .end_op = cache_end_op,
    .alloc = cache_alloc,
    .free = cache_free,
};
