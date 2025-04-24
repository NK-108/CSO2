#include "lab.h"

#include <string.h>     // for memset
#include <stdio.h>      // for printf

#define NUM_TRIALS 100

// Comparator for qsort
int compare_longs(const void *a, const void *b) {
    long x = *(const long *)a;
    long y = *(const long *)b;
    return (x > y) - (x < y);
}

// Measure median timing for one character guess at a given position
long median_time_for_guess(char *buffer, int pos, char guess_char) {
    long timings[NUM_TRIALS];
    buffer[pos] = guess_char;

    for (int i = 0; i < NUM_TRIALS; i++) {
        int result;
        timings[i] = measure_once(&result, buffer, check_passphrase);
    }

    qsort(timings, NUM_TRIALS, sizeof(long), compare_longs);
    return timings[NUM_TRIALS / 2];
}

/* When complete, this function should fill in "buffer"
 * with a length-character \0-termianted string such that
 * check_passphrase(buffer) is true.
 *
 * The implementation we supplied tries the guesses of
 * 'a', 'b', and 'c' and prints out how long it takes
 * to check each of them.
 *
 * To do so, your implementation should rely on timing
 * how long check_passphrase takes, most likely by using
 * "measure_once" wrapper function.
 *
 * (Your implementation may not examine the memory in which
 *  the passphrase is stored in another way.)
 */
void find_passphrase(char *buffer, int length) {
    char candidates[26];
    for (int i = 0; i < 26; i++) {
        candidates[i] = 'a' + i;
    }

    buffer[length] = '\0';

    while (1) { // Retry loop if final guess is incorrect
        memset(buffer, 0, length);

        for (int pos = 0; pos < length; pos++) {
            char best_char = 'a';
            long best_time = -1;

            // Interleaved timing loop
            long timings[26] = {0};
            for (int trial = 0; trial < NUM_TRIALS; trial++) {
                for (int i = 0; i < 26; i++) {
                    char c = candidates[i];
                    buffer[pos] = c;
                    int result;
                    long t = measure_once(&result, buffer, check_passphrase);
                    timings[i] += t;
                }
            }

            for (int i = 0; i < 26; i++) {
                long avg = timings[i] / NUM_TRIALS;
                if (avg > best_time) {
                    best_time = avg;
                    best_char = candidates[i];
                }
            }

            buffer[pos] = best_char;
        }

        // Confirm final guess
        int result;
        check_passphrase(buffer); // Ignore time, just check result
        measure_once(&result, buffer, check_passphrase);
        if (result) break; // Success!
    }
}


