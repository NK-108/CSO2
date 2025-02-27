#include <stdio.h>
#include <stdalign.h>
#include "mlpt.h"
#include "config.h"

size_t ptbr = 0;
int page_size;
int pte_size;
int pt_size;
int unused_bits;
int vpn_size;
int vpn_bits;

// For Testing Translate
alignas(4096)
static size_t testing_page_table[512];
static void set_testing_ptbr(void) {
    ptbr = (size_t) &testing_page_table[0];
}

void setup_test() {
    alignas(4096)
    static char data_for_page_3[4096];
    size_t address_of_data_for_page_3_as_integer = (size_t) &data_for_page_3[0];
    size_t physical_page_number_of_data_for_page_3 = address_of_data_for_page_3_as_integer >> 12;
        // instead of >> 12, we could have written:
            // address_of_data_for_page_3_as_integer / 4096
    size_t page_table_entry_for_page_3 = (
            // physical page number in upper (64-POBITS) bits
            (physical_page_number_of_data_for_page_3 << 12)
        |
            // valid bit in least significant bit, set to 1
            1
    );
    // assuming testing_page_table initialized as above and ptbr points to it
    testing_page_table[3] = page_table_entry_for_page_3;
    printf("Expected Address = %zx\n", &data_for_page_3[0x45]);
}

void set_variables() {
    page_size = 1 << POBITS;
    pte_size = 8;
    pt_size = page_size/pte_size;
    
    int temp = pt_size;
    int i = 0;
    while (temp != 1) {
        temp >>= 1;
        i += 1;
    }
    vpn_size = i;

    vpn_bits = vpn_size * LEVELS;
    unused_bits = 64 - vpn_bits - POBITS;
}

size_t translate(size_t va) {
    printf("Virtual Address = %zx\n", va);
    set_variables();

    // Isolate Page Offset
    size_t po = (va << (64 - POBITS)) >> (64 - POBITS);
    printf("Page Offset = %zx\n", po);

    // Isolate Virtual Page Numbers
    size_t vpn[LEVELS];
    for (int i = 0; i < LEVELS; ++i) {
        vpn[i] = ((va << unused_bits) << vpn_size * i) >> (64 - vpn_size);
        printf("Virtual Page Number %d = %zx\n", i + 1, vpn[i]);
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

    return level_base | po;

}

int main(void) {
    size_t test_address = 0x3045;
    set_testing_ptbr();
    setup_test();
    size_t return_address = translate(test_address);
    printf("Translated Address = %zx\n", return_address);
}
