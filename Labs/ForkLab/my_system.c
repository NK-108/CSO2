#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int my_system(const char *command) {
    
    pid_t childPID = fork();

    if (childPID == 0) {
        // child process
        int ex = execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        if (ex == -1) {
            // shell failed
            _exit(127);
        }
        else {
            // shell succeeded
            _exit(ex);
        }
    } 
    else if (childPID > 0) {
        // parent process
        return waitpid(childPID);
    }
    else {
        // fork failed
        return -1;
    }

}