#include "gc61.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

typedef struct mblock {
    uintptr_t ptr;
    size_t sz;
} mblock;

static mblock* blocks;
static size_t nblocks;
static size_t blocks_capacity;
static mblock* m61_find(void* ptr);

void* m61_malloc(size_t sz) {
    void* ptr = malloc(sz);
    if (ptr) {
        if (nblocks == blocks_capacity) {
            blocks_capacity = nblocks ? 2 * nblocks : 128;
            blocks = (mblock*)
                realloc(blocks, sizeof(mblock) * blocks_capacity);
            assert(blocks);
        }
        blocks[nblocks].ptr = (uintptr_t) ptr;
        blocks[nblocks].sz = sz;
        ++nblocks;
    }
    return ptr;
}

static mblock* m61_find(void* ptr) {
    for (size_t i = 0; i != nblocks; ++i) {
        if ((uintptr_t) ptr >= blocks[i].ptr
            && (uintptr_t) ptr < blocks[i].ptr + blocks[i].sz) {
            return &blocks[i];
        }
    }
    return NULL;
}

void m61_free(void* ptr) {
    if (ptr) {
        mblock* m = m61_find(ptr);
        assert(m && m->ptr == (uintptr_t) ptr);
        *m = blocks[nblocks - 1];
        --nblocks;
    }
    free(ptr);
}

void m61_print_allocations(void) {
    for (size_t i = 0; i != nblocks; ++i) {
        fprintf(stderr, "%p: allocated object with size %zu\n",
                (void*) blocks[i].ptr, blocks[i].sz);
    }
}

void m61_find_allocations(char* base, size_t sz) {
    (void) base, (void) sz;
}

void m61_cleanup(void) {
}