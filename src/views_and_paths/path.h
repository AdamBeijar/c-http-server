#ifndef PATH_H
#define PATH_H

typedef struct{
    int status_code;
    char *content_type;
    char *content;
} view_t;

char *find_view(struct request *req);

#endif