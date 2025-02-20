#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

char *getoutput(const char *command);
char *parallelgetoutput(int count, const char **argv_base);

char *getoutput(const char *command) {
    int pipe_fd[2];
    pipe(pipe_fd);
    int read_fd = pipe_fd[0];
    int write_fd = pipe_fd[1];

    pid_t child_pid = fork();
    if (child_pid == 0) {
        // child process
        dup2(write_fd, STDOUT_FILENO);

        close(read_fd);
        close(write_fd);

        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        exit(0);
    } else {
        // parent process
        close(write_fd);
        
        char *ptr = NULL;
        size_t n = 0;
        getdelim(&ptr, &n, (int) '\0', fdopen(read_fd, "r"));

        close(read_fd);

        waitpid(child_pid, NULL, 0);

        return ptr;
    }

}

char *parallelgetoutput(int count, const char **argv_base) {
    int pipe_fd[2];
    pipe(pipe_fd);
    int read_fd = pipe_fd[0];
    int write_fd = pipe_fd[1];
    pid_t child_pid[count];
    char *ptr = NULL;
    size_t n = 0;

    for (int i = 0; i < count; i++) {
        child_pid[i] = fork();
        if (child_pid[i] == 0) {
            // child process
            dup2(write_fd, STDOUT_FILENO);
            close(read_fd);
            close(write_fd);

            int len = 0;
            while (argv_base[len] != NULL) len++;
            const char *argv[len+2];
            for (int j = 0; j < len; j++) {
                argv[j] = argv_base[j];
            }
            char str[12];
            sprintf(str, "%d",i);
            argv[len] = str;
            argv[len+1] = NULL;

            execv(argv[0], argv);
            exit(0);
        }
    }

    // parent process
    close(write_fd);
    getdelim(&ptr, &n, (int) '\0', fdopen(read_fd, "r"));
    close(read_fd);

    for (int i = 0; i < count; i++) {
        waitpid(child_pid[i], NULL, 0);
    }

    return ptr;
}

// int main() {
//     //GETOUPUT
//     printf("Hi!\n");
//     printf("Text: [[[%s]]]\n", 
//         getoutput("echo 1 2 3; sleep 2; echo 5 5"));
//     printf("Bye!\n");

//     //GETPARALLELOUTPUT
//     const char *argv_base[] = {
//         "/bin/echo", "running", NULL
//     };
//     const char *output = parallelgetoutput(2, argv_base);

//     printf("Text: [%s]\n", output);
// }

