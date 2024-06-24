#include <string.h>

int includes(const char *str, const char *substr) {
    return strstr(str, substr) != NULL;
}