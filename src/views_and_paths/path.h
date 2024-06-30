#ifndef PATH_H
#define PATH_H

typedef struct{
    int status_code;
    char *content_type;
    char *content;
} view_t;

char *find_view(struct request *req);

char **open_html_file(char *html, int *line_count);

char *build_html_response(char **html, int *line_count);

#endif