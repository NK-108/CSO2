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
#define VPN_SIZE (POBITS - 3) // number of bits for each vpn level
#define VPN_BITS (VPN_SIZE * LEVELS) // number of bits in complete vpn
#define UNUSED_BITS (64 - VPN_BITS - POBITS) // number of unused bits in virtual address

size_t vpn[LEVELS]; // array of VPNs
int allocations = 0;

void set_vpn(size_t va) {
    //printf("Virtual Address = %zx\n", va);
    for (int i = 0; i < LEVELS; ++i) {
        vpn[i] = ((va << UNUSED_BITS) << VPN_SIZE * i) >> (64 - VPN_SIZE);
        //printf("Virtual Page Number %d = %zx\n", i + 1, vpn[i]);
    }
}

size_t translate(size_t va) {
    // initialize variables 
    set_vpn(va);
    int po =  va & ~PAGE_MASK;

    if (ptbr == 0) {
        return ~0;
    }

    // Perform Translation
    size_t* page_table = (size_t*) ptbr;
    for (int level = 0; level < LEVELS; ++level) {
        int index = vpn[level];
        if ((page_table[index] & 1) != 1) {
            // Valid Bit = 0
            return ~0;
        } else {
            page_table = (size_t*)(page_table[index] & PAGE_MASK);
        }
    }

    //printf("Translation: Address %zx => Address = %zx\n", va, (size_t)page_table | po);

    return (size_t)page_table | po;

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
    set_vpn(va);

    // Allocate the top-level page table if not already done
    if (ptbr == 0) {
        ptbr = (size_t)allocate_page();
        //printf("Allocation %d: ptbr = %zx\n", allocations, ptbr);
    }

    size_t* page_table = (size_t*) ptbr;
    for (int level = 0; level < LEVELS; ++level) {
        int index = vpn[level];
        if ((page_table[index] & 1) != 1) {
            // Valid Bit = 0
            page_table[index] = (size_t)allocate_page() | 1;
        }
        //printf("Allocation %d: Level %d PTE = %zx\n", allocations, level + 1, page_table[index]);

        page_table = (size_t*)(page_table[index] & PAGE_MASK);

    }
}

int main() {
    // 0 pages have been allocated
    assert(ptbr == 0);

    page_allocate(0x456789abcdef);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    page_allocate(0x456789abcd00);
    // no new pages allocated (still 5)

    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)


    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);

    page_allocate(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)
}