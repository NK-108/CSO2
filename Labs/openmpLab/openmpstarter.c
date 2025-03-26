#include <stdio.h> // fopen, fread, fclose, printf, fseek, ftell
#include <math.h> // log, exp
#include <stdlib.h> // free, realloc
#include <time.h> // struct timespec, clock_gettime, CLOCK_REALTIME
#include <errno.h>


// computes the geometric mean of a set of values.
// You should use OpenMP to make faster versions of this.
// Keep the underlying sum-of-logs approach.
double geomean(unsigned char *s, size_t n) {
    double answer = 0;
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) answer += log(s[i]) / n;
    }
    return exp(answer);
}

double geomeanEVEN_ATOMIC(unsigned char *s, size_t n) {
    double answer = 0;
    
    // Even Split Map - Atomic Reduction
    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) {
            #pragma omp atomic update
            answer += log(s[i]) / n;
        }
    }
    return exp(answer);
}

double geomeanEVEN_MtF(unsigned char *s, size_t n) {
    double answer = 0;
    
    // Even Split Map - MtF Reduction
    #pragma omp parallel
    {
        double local_answer = 0;
        #pragma omp for nowait
        for(int i=0; i<n; i+=1) {
            if (s[i] > 0) local_answer += log(s[i]) / n;
        }

        #pragma omp atomic update
        answer += local_answer;
    }
    return exp(answer);
}

double geomeanEVEN_BUILTIN(unsigned char *s, size_t n) {
    double answer = 0;
    
    // Even Split Map - Built-In Reduction
    #pragma omp parallel for reduction(+:answer)
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) {
            answer += log(s[i]) / n;
        }
    }
    return exp(answer);
}

double geomeanQUEUE_ATOMIC(unsigned char *s, size_t n) {
    double answer = 0;

    // Task Queue Map - Atomic Reduction
    int j = 0;
    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) {
            #pragma omp atomic update
            answer += log(s[i]) / n;
        }
    }
    return exp(answer);
}

double geomeanQUEUE_BUILTIN(unsigned char *s, size_t n) {
    double answer = 0;

    // Task Queue Map - Built-In Reduction
    int j = 0;
    #pragma omp parallel reduction(+:answer)
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) answer += log(s[i]) / n;
    }
    return exp(answer);
}

double geomeanQUEUE_MtF(unsigned char *s, size_t n) {
    double answer = 0;

    // Task Queue Map - MtF Reduction
    int j = 0;
    #pragma omp parallel
    {
        double local_answer = 0;
        #pragma omp nowait
        while (1) {
            int i;
            #pragma omp atomic capture
            i = j++;
            if (i >= n) break;
            if (s[i] > 0) local_answer += log(s[i]) / n;
        }

        #pragma omp atomic update
        answer += local_answer;
    }
    
    return exp(answer);
}

/// nanoseconds that have elapsed since 1970-01-01 00:00:00 UTC
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}


/// reads arguments and invokes geomean; should not require editing
int main(int argc, char *argv[]) {
    // step 1: get the input array (the bytes in this file)
    char *s = NULL;
    size_t n = 0;
    for(int i=1; i<argc; i+=1) {
        // add argument i's file contents (or string value) to s
        FILE *f = fopen(argv[i], "rb");
        if (f) { // was a file; read it
            fseek(f, 0, SEEK_END); // go to end of file
            size_t size = ftell(f); // find out how many bytes in that was
            fseek(f, 0, SEEK_SET); // go back to beginning
            s = realloc(s, n+size); // make room
            fread(s+n, 1, size, f); // append this file on end of others
            fclose(f);
            n += size; // not new size
        } else { // not a file; treat as a string
            errno = 0; // clear the read error
        }
    }

    // step 2: invoke and time the geometric mean function
    long long t0 = nsecs();
    // double answer = geomean((unsigned char*) s,n);               // 1047204564 ns
    // double answer = geomeanEVEN_ATOMIC((unsigned char*) s,n);    // 210079399 ns 
    // double answer = geomeanEVEN_BUILTIN((unsigned char*) s,n);   // 18904545 ns
    double answer = geomeanEVEN_MtF((unsigned char*) s,n);       // 15978161 ns
    // double answer = geomeanQUEUE_ATOMIC((unsigned char*) s,n);   // 492206916 ns
    // double answer = geomeanQUEUE_BUILTIN((unsigned char*) s,n);  // 176334968 ns
    // double answer = geomeanQUEUE_MtF((unsigned char*) s,n);       // 17415502 ns 
    long long t1 = nsecs();

    free(s);

    // step 3: report result
    printf("%lld ns to process %zd characters: %g\n", t1-t0, n, answer);
}