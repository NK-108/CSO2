#include "split.h"

static int is_separator(char c, const char *sep) {
    while (*sep) {
        if (c == *sep++) return 1;
    }
    return 0;
}

char **string_split(const char *input, const char *sep, int *num_words) {
    int capacity = 10;
    int count = 0;
    char **result = malloc(capacity * sizeof(char *));
    if (!result) return NULL;

    const char *p = input;
    if (is_separator(*p, sep)) { // Leading separator, create an empty word
        result[count] = malloc(1);
        if (!result[count]) return NULL;
        result[count][0] = '\0';
        count++;
        p++;
    }

    while (*p) {
        while (*p && is_separator(*p, sep)) p++; // Skip consecutive separators
        if (*p) {
            const char *start = p;
            while (*p && !is_separator(*p, sep)) p++;
            int len = p - start;
            
            if (count >= capacity) {
                capacity *= 2;
                result = realloc(result, capacity * sizeof(char *));
                if (!result) return NULL;
            }
            
            result[count] = malloc(len + 1);
            if (!result[count]) return NULL;
            strncpy(result[count], start, len);
            result[count][len] = '\0';
            count++;
        }
    }

    if (p > input && is_separator(*(p - 1), sep)) { // Trailing separator, create an empty word
        if (count >= capacity) {
            capacity *= 2;
            result = realloc(result, capacity * sizeof(char *));
            if (!result) return NULL;
        }
        result[count] = malloc(1);
        if (!result[count]) return NULL;
        result[count][0] = '\0';
        count++;
    }
    
    *num_words = count;
    return result;
}
