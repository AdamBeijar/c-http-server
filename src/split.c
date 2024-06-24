#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "string_help.h"

char **split(char *str, const char *delim, int *count) {
    char *tempString = strdup(str);
    if (tempString == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    int tokens = 0;
    char *tmp = tempString;
    while ((tmp = strpbrk(tmp, delim))) {
        tokens++;
        tmp++;
    }
    tokens++; // Account for the last token

    char **arrayOfStrings = malloc((tokens + 1) * sizeof(char *));
    if (arrayOfStrings == NULL) {
        perror("Failed to allocate memory");
        free(tempString);
        return NULL;
    }

    int i = 0;
    char *token = strtok(tempString, delim);
    while (token != NULL) {
        arrayOfStrings[i] = strdup(token);
        if (arrayOfStrings[i] == NULL) {
            perror("Failed to allocate memory");
            for (int j = 0; j < i; j++) {
                free(arrayOfStrings[j]);
            }
            free(arrayOfStrings);
            free(tempString);
            return NULL;
        }
        token = strtok(NULL, delim);
        i++;
    }
    arrayOfStrings[i] = NULL;
    *count = tokens;

    free(tempString);

    return arrayOfStrings;
}
