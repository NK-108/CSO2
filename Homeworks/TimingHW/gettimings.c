#include "gettimings.h"

long long time0;
long long time1;
long long duration;
long long overhead;

pid_t other_pid;
pid_t this_pid;

int main(int argc, char* argv[]) {
    
    long long duration;

    int scenario = atoi(argv[1]);

    // signal handler setp
    struct sigaction sa;
    sa.sa_handler = sig_handler5;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &sa, NULL);

    switch (scenario) {
        case 1:
            duration = scenario1();
            printf("Duration: %lld ns\n", duration);
            break;
        case 2:
            duration = scenario2();
            printf("Duration: %lld ns\n", duration);
            break;
        case 3:
            duration = scenario3();
            printf("Duration: %lld ns\n", duration);
            break;
        case 4:
            duration = scenario4();
            printf("Duration: %lld ns\n", duration);
            break;
        case 5:
            duration = scenario5();
            printf("Duration: %lld ns\n", duration);
            break;
        case -1:
            this_pid = getpid();
            other_pid;
            printf("PID: %d \n", this_pid);
            scanf("%d", &other_pid);
            break;
        default:
            printf("Invalid Scenario Number\n");
            break;
    }

}

// returns the number of nanoseconds that have elapsed since an arbitrary time
long long nsecs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec*1000000000 + t.tv_nsec;
}

// calculate nanoseconds to do 'nothing'
void set_overhead() {
    
    int i = 0;
    time0 = nsecs();
    while((i - 1) < 0) {
        i++;
        time1 = nsecs();
    }
    overhead = time1 - time0;

    printf("Overhead: %lld ns\n", overhead);
}

long long scenario1() {
    
    set_overhead();

    time0 = nsecs();
    int i = 0;
    while ((time1 - time0) < (long long) 500000000) {
        empty();
        time1 = nsecs();
        i++;
    }
    duration = (time1 - time0)/i - overhead;

    return duration;

}

long long scenario2() {

    set_overhead();

    time0 = nsecs();
    int i = 0;
    while ((time1 - time0) < (long long) 500000000) {
        getppid();
        time1 = nsecs();
        i++;
    }
    duration = (time1 - time0)/i - overhead;

    return duration;

}

long long scenario3() {

    set_overhead();

    time0 = nsecs();
    int i = 0;
    while ((time1 - time0) < (long long) 500000000) {
        system("/bin/true");
        time1 = nsecs();
        i++;
    }
    duration = (time1 - time0)/i - overhead;

    return duration;

}

long long scenario4() {

    set_overhead();

    // signal handler setp
    struct sigaction sa4;
    sa4.sa_handler = sig_handler4;
    sigemptyset(&sa4.sa_mask);
    sa4.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa4, NULL);

    pid_t pid = getpid();

    time0 = nsecs();
    int i = 0;
    while ((time1 - time0) < (long long) 500000000) {
        kill(pid, SIGUSR1);
        i++;
    }
    duration = (time1 - time0)/i - overhead;

    return duration;

}

long long scenario5() {

    set_overhead();

    pid_t this_pid = getpid();
    pid_t other_pid;
    printf("PID: %d\n", this_pid);
    scanf("%d", &other_pid);

    time0 = nsecs();
    int i = 0;
    while ((time1 - time0) < (long long) 500000000) {
        kill(other_pid, SIGUSR2);
        sigwait(SIGUSR2, SIGUSR2);
        time1 = nsecs();
        i++;
    }
    duration = (time1 - time0)/i - overhead;

    return duration;
}

__attribute__((noinline)) void empty() {
    __asm__("");
}

static void sig_handler4(int signum) {
    time1 = nsecs();
}

static void sig_handler5(int signum) {
    kill(other_pid, SIGUSR2);
}

