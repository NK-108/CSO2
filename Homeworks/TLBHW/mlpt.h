#ifndef MLPT_H
#define MLPT_H

#include <stdlib.h>
/**
 * Page table base register.
 * Declared here so tester code can look at it; because it is extern
 * you'll need to define it (without extern) in exactly one .c file.
 */
extern size_t ptbr;

/**
 * Given a virtual address, return the physical address.
 * Return a value consisting of all 1 bits
 * if this virtual address does not have a physical address.
 */
size_t translate(size_t va);

/**
 * Setup a mapping in the page tables pointed to by `ptbr` so that
 * the virtual page that the virtual address `va` is in
 * maps to a physical page allocated with `posix_memalign`
 * (unless such a mapping already exists).
 *
 * Any page tables not yet allocated will be allocated using `posix_memalign`.
 * (Any pages or page tables already created, such as by a prior
 * call to `page_allocate` will be reused. If the mapping is already entirely
 * setup, the function should do nothing.)
 */
void page_allocate(size_t va);

/*
* Navigates page tables allocated by page_allocate() to free data page 
* associated with virtual address 'va', storing table pointers and indices
* Then travels back up page table hierarchy and checks for empty page tables,
* which it also frees.
*/
void page_deallocate(size_t va);

/*
* Given virtual address 'va', calculates vpns for all levels and stores in global int array.
*/
void set_vpn(size_t va);

/*
* Allocates a single page using posix_memalign(), sets all data to 0, and increments global allocation counter.
*/
void* allocate_page();

/*
* Checks the contents of page table passed by 'page_table', returns 1 if empty, 0 if not.
*/
int is_page_table_empty(size_t* page_table);

#endif