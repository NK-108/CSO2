#include "split.h"

static int is_separator(char c, const char *sep) {
    while (*sep) {
        if (c == *sep++) return 1;
    }
    return 0;
}

char **string_split(const char *input, const char *sep, int *num_words) {
    int count = 1;
    for (const char *p = input; *p; p++) {
        if (is_separator(*p, sep)) {
            count++;
        }
    }

    char **result = malloc(count * sizeof(char *));
    if (!result) return NULL;

    *num_words = count;
    const char *start = input;
    int idx = 0;
    for (const char *p = input; ; p++) {
        if (is_separator(*p, sep) || *p == '\0') {
            int len = p - start;
            result[idx] = malloc(len + 1);
            if (!result[idx]) return NULL;
            strncpy(result[idx], start, len);
            result[idx][len] = '\0';
            idx++;
            if (*p == '\0') break;
            start = p + 1;
        }
    }
    return result;
}