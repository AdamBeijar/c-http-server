#ifndef PATH_H
#define PATH_H

#include "../request_type/request_type.h"

typedef struct{
    int status_code;
    char *content_type;
    char *content;
} view_t;

char *find_view(request_t *req);

char **open_html_file(char *html, int *line_count);

char *build_html_response(char **html, int *line_count);

#endif