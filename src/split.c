#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_help.h"

// Split a string into an array of strings based on a delimiter, like a normal split function in other languages
char **split(char *str, const char *delim, int *count) {
    char *tempString = strdup(str); // Duplicate the string to avoid modifying the original
    if (tempString == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    // Count tokens to know how much memory to allocate
    int tokens = 0;
    char *tmp = tempString;
    while ((tmp = strpbrk(tmp, delim))) {
        tokens++;
        tmp++;
    }
    tokens++; // Account for the last token

    // Allocate memory for array of strings
    char **arrayOfStrings = malloc((tokens + 1) * sizeof(char *));
    if (arrayOfStrings == NULL) {
        perror("Failed to allocate memory");
        free(tempString);
        return NULL;
    }

    // Tokenize the string and populate the array
    int i = 0;
    char *token = strtok(tempString, delim); // Get the first token
    while (token != NULL) {
        arrayOfStrings[i] = strdup(token); // Duplicate the token to avoid modifying the original
        if (arrayOfStrings[i] == NULL) {
            perror("Failed to allocate memory");
            for (int j = 0; j < i; j++) {
                free(arrayOfStrings[j]);
            }
            free(arrayOfStrings);
            free(tempString);
            return NULL;
        }
        token = strtok(NULL, delim); // Get the next token
        i++;
    }
    arrayOfStrings[i] = NULL; // Null-terminate the array
    *count = tokens; // Set the count of tokens

    free(tempString); // Free temporary string

    return arrayOfStrings; // Return array of strings
}