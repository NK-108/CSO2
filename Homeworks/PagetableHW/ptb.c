#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdalign.h>
#include <assert.h>
#include <string.h>
#include "mlpt.h"
#include "config.h"

size_t ptbr = 0;

#define PAGE_SIZE (1 << POBITS) // number of bytes per page
#define PAGE_MASK (~(PAGE_SIZE - 1)) 
#define PTE_SIZE 8 // number of bytes per page table entry
#define PT_LENGTH (PAGE_SIZE/PTE_SIZE) // number of page table entries

int UNUSED_BITS = 0; // number of unused bits in virtual address
int VPN_SIZE = 0; // number of bits in complete vpn
int VPN_BITS = 0; // number of bits for each vpn level
size_t vpn[LEVELS]; // array of VPNs
size_t po = 0; // page offset
int allocations = 0;

void set_var(size_t va) {
    
    int temp = PT_LENGTH;
    int i = 0;
    while (temp != 1) {
        temp >>= 1;
        i += 1;
    }
    VPN_SIZE = i;
    VPN_BITS = VPN_SIZE * LEVELS;
    UNUSED_BITS = 64 - VPN_BITS - POBITS;

    // Isolate Page Offset
    po = va & ~PAGE_MASK;
    //printf("Page Offset = %zx\n", po);

    // Isolate Virtual Page Numbers
    for (int i = 0; i < LEVELS; ++i) {
        vpn[i] = ((va << UNUSED_BITS) << VPN_SIZE * i) >> (64 - VPN_SIZE);
        //printf("Virtual Page Number %d = %zx\n", i + 1, vpn[i]);
    }
}

size_t translate(size_t va) {
    // initialize variables 
    set_var(va);

    if (ptbr == 0) {
        return ~0;
    }

    // Perform Translation
    size_t level_base = ptbr;
    size_t pte;
    for (int i = 0; i < LEVELS; ++i) {
        pte = *((size_t*) (level_base + (vpn[i] * 8)));
        if ((pte & 1) != 1) {
            // Valid Bit = 0
            return ~0;
        } else {
            level_base = (pte >> POBITS) << POBITS;
        }
    }

    printf("Translation: Address %zx => Address = %zx\n", va, level_base | po);

    return level_base | po;

}

void* allocate_page() {
    void* ptr = NULL;
    if (posix_memalign(&ptr, PAGE_SIZE, PAGE_SIZE) != 0) {
        //printf("posix_memalign failed");
        return NULL;
    }
    memset(ptr, 0, PAGE_SIZE);
    allocations += 1;
    return ptr;
}

void page_allocate(size_t va) {
    // Initialize variables
    set_var(va);

    // Allocate the top-level page table if not already done
    if (ptbr == 0) {
        ptbr = (size_t)allocate_page();
        printf("Allocation %d: ptbr = %zx\n", allocations, ptbr);
    }

    size_t* page_table = (size_t*) ptbr;
    for (int level = 0; level < LEVELS; ++level) {
        int index = vpn[level];
        if ((*(page_table + index) & 1) != 1) {
            // Valid Bit = 0
            page_table[index] = (size_t)allocate_page() | 1;
        }
        printf("Allocation %d: Level %d PTE = %zx\n", allocations, level + 1, page_table[index]);

        page_table = (size_t*)(page_table[index] & PAGE_MASK);

    }
}
