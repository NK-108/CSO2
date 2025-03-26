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
    double *answer = malloc(n * sizeof(double));
    
    // Even Split Map
    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) answer[i] = log(s[i]) / n;
    }

    // Atomic Reduction
    double result = 0;
    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        #pragma omp atomic update
        result += answer[i];
    }

    free(answer);
    return exp(result);
}

double geomeanEVEN_MtF(unsigned char *s, size_t n) {
    double *answer = malloc(n * sizeof(double));
    
    // Even Split Map
    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) answer[i] = log(s[i]) / n;
    }

    // Many-to-Few Reduction
    double result = 0;
    #pragma omp parallel
    {
        double local_result = 0;
        #pragma omp for nowait
        for(int i=0; i<n; i+=1) {
            local_result += answer[i];
        }

        #pragma omp atomic update
        result += local_result;
    }

    free(answer);
    return exp(result);
}

double geomeanEVEN_BUILTIN(unsigned char *s, size_t n) {
    double *answer = malloc(n * sizeof(double));
    
    // Even Split Map
    #pragma omp parallel for
    for(int i=0; i<n; i+=1) {
        if (s[i] > 0) answer[i] = log(s[i]) / n;
    }

    // Built-In Reduction
    double result = 0;
    #pragma omp parallel for reduction(op:result)
    for(int i=0; i<n; i+=1) {
        result += answer[i];
    }

    free(answer);
    return exp(result);
}

double geomeanQUEUE_ATOMIC(unsigned char *s, size_t n) {
    double *answer = malloc(n * sizeof(double));

    // Task Queue Map
    int j = 0;
    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) answer[i] = log(s[i]) / n;
    }

    // Atomic Reduction
    double result = 0;
    j = 0;
    #pragma omp parallel 
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        #pragma omp atomic update
        result += answer[i];
    }

    free(answer);
    return exp(result);
}

double geomeanQUEUE_BUILTIN(unsigned char *s, size_t n) {
    double *answer = malloc(n * sizeof(double));

    // Task Queue Map
    int j = 0;
    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) answer[i] = log(s[i]) / n;
    }

    // Built-In Reduction
    double result = 0;
    j = 0;
    #pragma omp parallel reduction(op:result)
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        result += answer[i];
    }

    free(answer);
    return exp(result);
}

double geomeanQUEUE_MtF(unsigned char *s, size_t n) {
    double *answer = malloc(n * sizeof(double));

    // Task Queue Map
    int j = 0;
    #pragma omp parallel
    while (1) {
        int i;
        #pragma omp atomic capture
        i = j++;
        if (i >= n) break;
        if (s[i] > 0) answer[i] = log(s[i]) / n;
    }

    // Many-to-Few Reduction
    double result = 0;
    j = 0;
    #pragma omp parallel 
    {
        double local_result = 0;
        #pragma omp for nowait
        while (1) {
            int i;
            #pragma omp atomic capture
            i = j++;
            if (i >= n) break;
            local_result += answer[i];
        }

        #pragma omp atomic update
        result += local_result;
    }
    

    free(answer);
    return exp(result);
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
    double answer = geomean((unsigned char*) s,n);
    // double answer = geomeanEVEN_ATOMIC((unsigned char*) s,n);
    // double answer = geomeanEVEN_BUILTIN((unsigned char*) s,n);
    // double answer = geomeanEVEN_MtF((unsigned char*) s,n);
    // double answer = geomeanQUEUE_ATOMIC((unsigned char*) s,n);
    // double answer = geomeanQUEUE_BUILTIN((unsigned char*) s,n);
    // double answer = geomeanEVEN_MtF((unsigned char*) s,n);
    long long t1 = nsecs();

    free(s);

    // step 3: report result
    printf("%lld ns to process %zd characters: %g\n", t1-t0, n, answer);
}