#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "string_help.h"

char *strip(char *str) {
    size_t start, end;

    // Find the first non-whitespace character from the beginning
    for (start = 0; isspace((unsigned char)str[start]); ++start);

    // Find the last non-whitespace character from the end
    end = strlen(str);
    while (end > 0 && isspace((unsigned char)str[end - 1]))
        --end;

    // Shift the content to remove leading whitespace
    size_t len = end - start;
    memmove(str, str + start, len);

    // Null-terminate the result
    str[len] = '\0';
    return str;
}