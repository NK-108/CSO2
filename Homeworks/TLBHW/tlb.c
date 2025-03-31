#include <stdio.h>
#include <stdalign.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "tlb.h"

#define WAYS 4
#define SETS 16
#define ENTRIES (WAYS * SETS)
#define INDEX_BITS (int)log2(SETS)
#define INDEX_MASK (((1 << (INDEX_BITS + POBITS)) - 1) & (((1 << INDEX_BITS) - 1) << POBITS))
#define TAG_MASK ~(((1 << (INDEX_BITS + POBITS)) - 1))

typedef struct {
    int valid;
    size_t vpn;
    size_t pa;
    int lru_rank;  // 1 = most recently used, 4 = least recently used
} TLBEntry;

typedef struct {
    TLBEntry entries[WAYS];
} TLBSet;

static TLBSet tlb[SETS];

/** Helper function to get the set index from a virtual address */
static size_t get_set_index(size_t va) {
    return (va & INDEX_MASK) >> POBITS; 
}

/** Helper function to update LRU ranking in a set */
static void update_lru(TLBSet *set, int accessed_index) {
    int old_rank = set->entries[accessed_index].lru_rank;

    for (int i = 0; i < WAYS; i++) {
        if (set->entries[i].valid && set->entries[i].lru_rank < old_rank) {
            set->entries[i].lru_rank++;  // Move older entries down
        }
    }
    set->entries[accessed_index].lru_rank = 1;  // Mark as most recently used
}

/** invalidate all cache lines in the TLB */
void tlb_clear() {
    memset(tlb, 0, sizeof(tlb));  // Zero out all entries
}

/**
 * return 0 if this virtual address does not have a valid
 * mapping in the TLB. Otherwise, return its LRU status: 1
 * if it is the most-recently used, 2 if the next-to-most,
 * etc.
 */
int tlb_peek(size_t va) {
    size_t set_index = get_set_index(va);
    TLBSet *set = &tlb[set_index];
    size_t vpn = va >> POBITS;

    for (int i = 0; i < WAYS; i++) {
        if (set->entries[i].valid && set->entries[i].vpn == vpn) {
            return set->entries[i].lru_rank;
        }
    }
    return 0;  // Not found
}

// STUB FOR TESTING
size_t tlb_translate(size_t va) {
    if (va < 0x1234000)
        return va + 0x20000;
    else if (va > 0x2000000 && va < 0x2345000)
        return va + 0x100000;
    else
        return -1;
}

int main (void) {
    tlb_clear();
    assert(tlb_peek(0) == 0);
    assert(tlb_translate(0) == 0x0020000);
    assert(tlb_peek(0) == 1);
    assert(tlb_translate(0x200) == 0x20200);
    assert(tlb_translate(0x400) == 0x20400);
    assert(tlb_peek(0) == 1);
    assert(tlb_peek(0x200) == 1);
    assert(tlb_translate(0x2001200) == 0x2101200);
    assert(tlb_translate(0x0005200) == 0x0025200);
    assert(tlb_translate(0x0008200) == 0x0028200);
    assert(tlb_translate(0x0002200) == 0x0022200);
    assert(tlb_peek(0x2001000) == 1);
    assert(tlb_peek(0x0001000) == 0);
    assert(tlb_peek(0x0004000) == 0);
    assert(tlb_peek(0x0005000) == 1);
    assert(tlb_peek(0x0008000) == 1);
    assert(tlb_peek(0x0002000) == 1);
    assert(tlb_peek(0x0000000) == 1);
    tlb_clear();
    assert(tlb_peek(0x2001000) == 0);
    assert(tlb_peek(0x0005000) == 0);
    assert(tlb_peek(0x0008000) == 0);
    assert(tlb_peek(0x0002000) == 0);
    assert(tlb_peek(0x0000000) == 0);
    assert(tlb_translate(0) == 0x20000);
    assert(tlb_peek(0) == 1);
}