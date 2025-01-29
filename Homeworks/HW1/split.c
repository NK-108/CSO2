#include "split.h"
static int is_separator(char c, const char *sep) {
    while (*sep) {
        if (c == *sep++) return 1;
    }
    return 0;
}

char **string_split(const char *input, const char *sep, int *num_words) {
    int count = 0;
    const char *p = input;
    while (*p) {
        while (*p && is_separator(*p, sep)) p++; // Skip consecutive separators
        if (*p) {
            count++;
            while (*p && !is_separator(*p, sep)) p++;
        }
    }

    char **result = malloc((count + 1) * sizeof(char *));
    if (!result) return NULL;

    *num_words = count;
    p = input;
    int idx = 0;
    while (*p) {
        while (*p && is_separator(*p, sep)) p++; // Skip consecutive separators
        if (*p) {
            const char *start = p;
            while (*p && !is_separator(*p, sep)) p++;
            int len = p - start;
            result[idx] = malloc(len + 1);
            if (!result[idx]) return NULL;
            strncpy(result[idx], start, len);
            result[idx][len] = '\0';
            idx++;
        }
    }
    result[count] = NULL; // Null-terminate array
    return result;
}
