#include "my_system.h"

int main(int argc, const char *argv[]) {
    // Test 1
    // int a1 = my_system("sleep 1; echo hi");
    // int a2 = my_system("echo bye");
    // int a3 = my_system("flibbertigibbet 23");
    // printf("%d %d %d\n", 
    //     WEXITSTATUS(a1), WEXITSTATUS(a2), WEXITSTATUS(a3));

    // Test 2
    my_system("echo -n 'Type something: ';" 
        " read got;"
        " echo Thanks for typing \\\"\"$got\"\\\"");
}