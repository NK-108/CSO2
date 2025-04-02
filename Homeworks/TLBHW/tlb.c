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
#define PAGE_MASK ((1 << POBITS) - 1)
#define INDEX_MASK (((1 << (INDEX_BITS + POBITS)) - 1) & (((1 << INDEX_BITS) - 1) << POBITS))
#define TAG_MASK ~(((1 << (INDEX_BITS + POBITS)) - 1))

typedef struct {
    int valid;
    size_t tag;
    size_t pa;
    int lru_rank;  // 1 = most recently used, 4 = least recently used
} TLBEntry;

typedef struct {
    TLBEntry entries[WAYS];
} TLBSet;

static TLBSet tlb[SETS];

/** Helper function to update LRU ranking in a set */
static void update_lru(TLBSet *set, int accessed_index) {
    int old_rank = set->entries[accessed_index].lru_rank;
    if (old_rank == 0) {
        old_rank = 4;
    }

    for (int i = 0; i < WAYS; i++) {
        if (set->entries[i].valid && set->entries[i].lru_rank <= old_rank) {
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
    size_t set_index = (va & INDEX_MASK) >> POBITS;
    TLBSet *set = &tlb[set_index];
    size_t tag = va >> (INDEX_BITS + POBITS);
    size_t po = va & PAGE_MASK;
    printf("PEEK => Virtual Address = 0x%zx, Index = 0x%zx, Tag = 0x%zx, Page Offset = 0x%zx\n", va, set_index, tag, po);


    for (int i = 0; i < WAYS; i++) {
        if (set->entries[i].valid && set->entries[i].tag == tag) {
            //printf("Found Entry for VA = %zx in WAY = %d\n", va, i);
            return set->entries[i].lru_rank;
        }
    }
    return 0;  // Not found
}

/** Perform a TLB lookup, updating LRU order, and inserting if necessary */
size_t tlb_translate(size_t va) {
    size_t set_index = (va & INDEX_MASK) >> POBITS;
    TLBSet *set = &tlb[set_index];
    size_t tag = va >> (POBITS + INDEX_BITS);
    size_t po = va & PAGE_MASK;
    printf("TRANSLATE => Virtual Address = 0x%zx, Index = 0x%zx, Tag = 0x%zx, Page Offset = 0x%zx => \n", va, set_index, tag, po);

    // Check if the VA is already in the TLB
    for (int i = 0; i < WAYS; i++) {
        if (set->entries[i].valid && set->entries[i].tag == tag) {
            update_lru(set, i);
            return (set->entries[i].pa & ~PAGE_MASK) + po;
        }
    }

    // Not found, get the translation
    size_t translation = translate(va & ~PAGE_MASK);
    size_t pa = translation + po;
    if (translation == (size_t)-1) {
        return (size_t)-1;  // Do not update TLB if translation fails
    }

    // Find an invalid entry or the least recently used one
    int lru_index = -1;
    int max_lru_rank = -1;
    for (int i = 0; i < WAYS; i++) {
        if (!set->entries[i].valid) {
            lru_index = i;
            break;
        }
        if (set->entries[i].lru_rank > max_lru_rank) {
            max_lru_rank = set->entries[i].lru_rank;
            lru_index = i;
        }
    }

    // Replace the selected entry
    set->entries[lru_index].valid = 1;
    set->entries[lru_index].tag = tag;
    set->entries[lru_index].pa = pa;

    // Update LRU tracking
    update_lru(set, lru_index);

    return pa;
}

size_t translate(size_t va) {
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

    tlb_clear();
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0801200) == 0x0821200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    assert(tlb_translate(0x0501200) == 0x0521200);
    assert(tlb_translate(0x0A01200) == 0x0A21200);
    assert(tlb_peek(0x0001200) == 0);
    assert(tlb_peek(0x2101200) == 0);
    assert(tlb_peek(0x2301200) == 3);
    assert(tlb_peek(0x0501200) == 2);
    assert(tlb_peek(0x0801200) == 4);
    assert(tlb_peek(0x0A01200) == 1);
    assert(tlb_translate(0x2301800) == 0x2401800);
    assert(tlb_peek(0x0001000) == 0);
    assert(tlb_peek(0x2101000) == 0);
    assert(tlb_peek(0x2301000) == 1);
    assert(tlb_peek(0x0501000) == 3);
    assert(tlb_peek(0x0801000) == 4);
    assert(tlb_peek(0x0A01000) == 2);
    for (int i = 0; i < 16; i += 1) {
        if (i != 1) {
            assert(tlb_translate(0x400000 + (i << 12)) == (0x420000 + (i << 12)));
            assert(tlb_translate(0x500000 + (i << 12)) == (0x520000 + (i << 12)));
            assert(tlb_translate(0x600000 + (i << 12)) == (0x620000 + (i << 12)));
            assert(tlb_translate(0x700000 + (i << 12)) == (0x720000 + (i << 12)));
            assert(tlb_translate(0x800000 + (i << 12)) == (0x820000 + (i << 12)));
            assert(tlb_peek(0x800000 + (i << 12)) == 1);
            assert(tlb_peek(0x400000 + (i << 12)) == 0);
            assert(tlb_peek(0x0001000) == 0);
            assert(tlb_peek(0x2101000) == 0);
            assert(tlb_peek(0x2301000) == 1);
            assert(tlb_peek(0x0501000) == 3);
            assert(tlb_peek(0x0801000) == 4);
            assert(tlb_peek(0x0A01000) == 2);
        }
    }
    tlb_clear();
    assert(tlb_peek(0x301000) == 0);
    assert(tlb_peek(0x501000) == 0);
    assert(tlb_peek(0x801000) == 0);
    assert(tlb_peek(0xA01000) == 0);
    assert(tlb_translate(0xA01200) == 0xA21200);

    tlb_clear();
    assert(tlb_translate(0xA0001200) == -1);
    assert(tlb_peek(0xA0001000) == 0);
    assert(tlb_translate(0x1200) == 0x21200);
    assert(tlb_peek(0xA0001200) == 0);
    assert(tlb_peek(0x1000) == 1);
    assert(tlb_translate(0xA0001200) == -1);
    assert(tlb_translate(0xB0001200) == -1);
    assert(tlb_translate(0xC0001200) == -1);
    assert(tlb_translate(0xD0001200) == -1);
    assert(tlb_translate(0xE0001200) == -1);
    assert(tlb_peek(0x1000) == 1);
    assert(tlb_translate(0x1200) == 0x21200);

    tlb_clear();
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0801200) == 0x0821200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    tlb_clear();
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x0001200) == 0x0021200);
    assert(tlb_translate(0x2101200) == 0x2201200);
    assert(tlb_translate(0x2301200) == 0x2401200);
    assert(tlb_translate(0x0011200) == 0x0031200);
    assert(tlb_peek(0x0001200) == 4);
    assert(tlb_peek(0x2101200) == 3);
    assert(tlb_peek(0x2301200) == 2);
    assert(tlb_peek(0x0011200) == 1);
}   