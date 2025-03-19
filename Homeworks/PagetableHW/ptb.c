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


void set_vpn(size_t va) {
    
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
    printf("Page Offset = %zx\n", po);

    // Isolate Virtual Page Numbers
    for (int i = 0; i < LEVELS; ++i) {
        vpn[i] = ((va << UNUSED_BITS) << VPN_SIZE * i) >> (64 - VPN_SIZE);
        printf("Virtual Page Number %d = %zx\n", i + 1, vpn[i]);
    }
}

size_t translate(size_t va) {
    printf("Virtual Address = %zx\n", va);

    // initialize variables if needed
    if (VPN_BITS == 0) {
        set_vpn(va);
    }

    // Perform Translation
    size_t level_base = ptbr;
    size_t pte;
    for (int level = 0; level < LEVELS; ++level) {
        pte = *((size_t*) (level_base + (vpn[level] * 8)));
        if ((pte & 1) != 1) {
            // Valid Bit = 0
            return ~0;
        } else {
            level_base = (pte >> POBITS) << POBITS;
        }
    }

    return level_base | po;

}

void* allocate_page() {
    void* ptr = NULL;
    if (posix_memalign(&ptr, PAGE_SIZE, PAGE_SIZE) != 0) {
        printf("posix_memalign failed");
        return NULL;
    }
    memset(ptr, 0, PAGE_SIZE);
    return ptr;
}

void page_allocate(size_t va) {
    // Initialize variables if needed
    if (VPN_BITS == 0) {
        set_vpn(va);
    }

    // Allocate the top-level page table if not already done
    if (ptbr == 0) {
        ptbr = (size_t)allocate_page();
    }

    size_t* current_table = (size_t*)ptbr;

    // Iterate through levels
    for (int level = 0; level < LEVELS; ++level) {
        size_t index = vpn[level];

        // Check if the next-level table exists
        if ((current_table[index] & 1) == 0) {
            // Allocate the next-level page table
            size_t* next_table = allocate_page();
            if (next_table == NULL) {
                printf("Failed to allocate next-level page table\n");
                return;
            }
            //printf("Level %d Allocated = %zx\n", level + 1, (size_t) next_table);
            current_table[index] = (size_t)next_table | 1;
        }

        // Move to the next level table
        current_table = (size_t*)current_table[index];
    }

    // Allocate the final physical page if not already done
    if (current_table[0] == 0) {
        size_t* physical_page = allocate_page();
        if (physical_page == NULL) {
            printf("Failed to allocate physical page\n");
            return;
        }
        //printf("Physical Page: %zx\n", (size_t) physical_page);
        current_table[0] = (size_t)physical_page | 1;
    }
}


