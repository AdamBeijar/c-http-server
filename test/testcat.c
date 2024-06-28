#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "src/string_help.h"

int main() {
    char boundary_prefix[] = "boundary=----------------------------648762028569933361802298";
    int word_count;
    char *boundary = split(boundary_prefix, "=", &word_count)[1];
    char *boundaryend = strdup(boundary);
    char *boundaryendcat = strdup("--");
    boundaryend = realloc(boundaryend, strlen(boundaryend) + strlen(boundaryendcat) + 1);
    strcat(boundaryend, boundaryendcat);
    printf("Boundary: %s\n", boundary);
    printf("Boundaryend: %s\n", boundaryend);
    return 0;
}
