#include "gettimings.h"

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

static void sig_handler5(int signum) {
    kill(other_pid, SIGUSR2);
}