#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../request_type/request_type.h"
#include "path.h"

view_t *index_view(struct request *req) {
    printf("Finding view for %s\n", req->path);
    char *name;
    printf("Data size: %d\n", req->data_size);
    if (req->data_size == 0) {
        name = malloc(6);
        name = "World";
        printf("No data found, using default: %s\n", name);
    } else {
        for (int i = 0; i < req->data_size; i++) {
            if (strcmp(req->data_union.form_data[i].key, "name") == 0) {
                name = malloc(strlen(req->data_union.form_data[i].value) + 1);
                name = req->data_union.form_data[i].value;
            }
        }
    }
    printf("Name found: %s\n", name);
    int status_code = 200;
    char *content_type = "text/html";
    char *content = malloc(strlen(name) + 1);
    sprintf(content, "<html><head><title>%s</title></head><body><h1>Hello, %s</h1></body></html>", name, name);
    view_t *return_view = malloc(sizeof(view_t));

    if(content == NULL) {
        perror("malloc");
        exit(1);
    }

    return_view->status_code = status_code;
    return_view->content_type = content_type;
    return_view->content = content;

    return return_view;
}
