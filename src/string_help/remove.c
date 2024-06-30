#include <string.h>
#include <stdio.h>

// Remove the number of occurrences of subStr from str
void removeSubString(char *str, char *subStr, int count) {
    int len = strlen(subStr);
    char *p = strstr(str, subStr);

    if(p != NULL){
        for (size_t i = 0; i < count; i++)
        {
            if(p != NULL){
                memmove(p, p + len, strlen(p + len) + 1); // +1 to copy the null terminator
                p = strstr(str, subStr); // Find the next occurrence
            }
        }
    }
}

// Remove all occurrences of subStr from str
void removeAllSubString(char *str, char *subStr){
    int len = strlen(subStr);
    char *p = strstr(str, subStr);

    while (p != NULL)
    {
        memmove(p, p + len, strlen(p + len) + 1); // +1 to copy the null terminator
        p = strstr(str, subStr); // Find the next occurrence
    }
}
