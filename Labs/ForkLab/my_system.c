#include "my_system.h"

int my_system(const char *command) {
    
    pid_t childPID = fork();
    int wstatus;

    if (childPID == 0) {
        // child process
        int ex = execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        if (ex == -1) {
            // shell failed
            _exit(127);
        }
        else {
            // shell succeeded
            _exit(0);
        }
    } 
    else if (childPID > 0) {
        // parent process
        return waitpid(childPID, &wstatus, 0);
    }
    else {
        // fork failed
        return -1;
    }

}