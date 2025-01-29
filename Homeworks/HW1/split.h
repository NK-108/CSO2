#ifndef SPLIT_H
#define SPLIT_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char **string_split(const char *input, const char *sep, int *num_words);

#endif 