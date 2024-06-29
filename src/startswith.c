#include <stdbool.h>
#include <string.h>
#include "string_help.h"

// Check if a string starts with a prefix
int startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre), // Get the length of the prefix
           lenstr = strlen(str); // Get the length of the string
    return lenstr < lenpre ? false : strncmp(pre, str, lenpre) == 0; // Compare the prefix to the string
}