#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *replace(char *str, char *old, char *new) {
    char *result;
    int i, count = 0;
    int new_len = strlen(new);
    int old_len = strlen(old);

    // Count the number of times the old string occurs in the string
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], old) == &str[i]) {
            count++;
            i += old_len - 1;
        }
    }

    // Allocate memory for the new string
    result = (char *)malloc(i + count * (new_len - old_len) + 1);

    i = 0;
    while (*str) {
        // Compare the substring with the result
        if (strstr(str, old) == str) {
            strcpy(&result[i], new);
            i += new_len;
            str += old_len;
        } else {
            result[i++] = *str++;
        }
    }

    result[i] = '\0';
    return result;
}