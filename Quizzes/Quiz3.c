#include <signal.h>
#include <process.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

int main(void) {
    pid_t pid;
    char str[20] = "foo";
    pid = fork();

    if (pid == 0) {
        strcat(str, "bar");
        printf("%s\n", str);
        exit(0);
    } else {
        strcat(str, "quux");
        printf("%s\n", str);
    }
    printf("%s\n", str);
}