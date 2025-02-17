#include "gettimings.h"

// returns the number of nanoseconds that have elapsed since an arbitrary time
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}

long scenario1(void) {

}

long scenario2(void) {

}

long scenario3(void) {

}

long scenario4(void) {

}

long scenario5(void) {

}