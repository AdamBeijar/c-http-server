#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "string_help.h"

// Check if a string starts with a prefix
int startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre), // Get the length of the prefix
           lenstr = strlen(str); // Get the length of the string
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0; // Compare the prefix to the string
}

int startWithStr(const char *pre, const char *str) {
    size_t lenpre = strlen(pre), // Get the length of the prefix
           lenstr = strlen(str); // Get the length of the string
    if (lenstr < lenpre) {
        return 0;
    }

    for (int i = 0; i < lenpre; i++) {
        if (pre[i] != str[i]) {
            return 0;
        }
    }
    
    return 1;
}