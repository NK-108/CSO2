#include "split.h"

int main(int argc, char *argv[]) {
    char sep[1024] = " \t";
    if (argc > 1) {
        sep[0] = '\0';
        for (int i = 1; i < argc; i++) {
            strcat(sep, argv[i]);
        }
    }

    char input[4001];
    while (fgets(input, sizeof(input), stdin)) {
        if (input[0] == '.' && input[1] == '\n') break;

        int num_words = 0;
        char **words = string_split(input, sep, &num_words);
        if (!words) continue;

        for (int i = 0; i < num_words; i++) {
            printf("[%s]", words[i]);
            free(words[i]);
        }
        printf("\n");
        free(words);
    }
    return 0;
}
