#define _GNU_SOURCE
#include "util.h"
#include <stdio.h>      // for printf
#include <stdlib.h>     // for atoi (and malloc() which you'll likely use)
#include <sys/mman.h>   // for mmap() which you'll likely use
#include <stdalign.h>

alignas(4096) volatile char global_array[4096 * 32]; 

void labStuff(int which) {
    if (which == 0) {
        /* do nothing */
    } else if (which == 1) {
        global_array[0 + (4096 * 0)] = 'a'; //new page -> increase faults by 1
        global_array[1 + (4096 * 0)] = 'a'; //same page -> no fault increase

        global_array[0 + (4096 * 1)] = 'a'; //new page -> increase faults by 1
        global_array[1 + (4096 * 1)] = 'a'; //same page -> no fault increase

        global_array[0 + (4096 * 2)] = 'a'; //new page -> increase faults by 1
        global_array[1 + (4096 * 2)] = 'a'; //same page -> no fault increase
    } else if (which == 2) {
        char *ptr = (char *)malloc(1048576); //allocate 1 MB
        ptr[0] = 'a';
    } else if (which == 3) {
        char *ptr = mmap(NULL,
                         1048576, 
                         PROT_READ | PROT_WRITE, 
                         MAP_PRIVATE | MAP_ANONYMOUS, 
                         -1, 
                         0
        );
        for (int i = 0; i < 32; i++) {
            ptr[0 + (4096 * i)] = 'a';
        }
    } else if (which == 4) {
        char *ptr = mmap((void *) 0x55555575a000,
                         4096, 
                         PROT_READ | PROT_WRITE, 
                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, 
                         -1, 
                         0
        );
        ptr[0] = 'a';
    } else if (which == 5) {
        char *ptr = mmap((void *) 0x56555555a000,
                         4096, 
                         PROT_READ | PROT_WRITE, 
                         MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, 
                         -1, 
                         0
        );
        ptr[0] = 'a';
    }
}

int main(int argc, char **argv) {
    int which = 0;
    if (argc > 1) {
        which = atoi(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        return 1;
    }
    printf("Memory layout:\n");
    print_maps(stdout);
    printf("\n");
    printf("Initial state:\n");
    force_load();
    struct memory_record r1, r2;
    record_memory_record(&r1);
    print_memory_record(stdout, NULL, &r1);
    printf("---\n");

    printf("Running labStuff(%d)...\n", which);

    labStuff(which);

    printf("---\n");
    printf("Afterwards:\n");
    record_memory_record(&r2);
    print_memory_record(stdout, &r1, &r2);
    print_maps(stdout);
    return 0;
}
