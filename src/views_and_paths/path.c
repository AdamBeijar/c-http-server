#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../request_type/request_type.h"
#include "views.h"

typedef struct path_t {
    char *path;
    char *method;
    view_t *(*view)(struct request *req);
} path_t;

char *create_response(view_t view) {
    char *response = malloc(strlen(view.content) + 100);

    if(response == NULL) {
        perror("malloc");
        exit(1);
    }

    sprintf(response, "HTTP/1.1 %d OK\nContent-Type: %s\n\n%s", view.status_code, view.content_type, view.content);
    return response;
}

char *find_view(struct request *req) {
    printf("Finding view for %s\n", req->path);
    path_t paths[] = {
        {"/", "GET", index_view},
    };
    char *path = req->path;
    size_t path_len = strlen(path);
    if (path[path_len - 1] == '/') {
        path[path_len - 1] = '\0';
    }
    for (int i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        char *current_path = strdup(paths[i].path);
        size_t current_path_len = strlen(current_path);
        if (current_path[current_path_len - 1] == '/') {
            current_path[current_path_len - 1] = '\0';
        }
        if (strcmp(current_path, path) == 0 && strcmp(paths[i].method, req->method) == 0) {
            printf("Found view for %s\n", req->path);
            view_t *view_response = paths[i].view(req);
            char *response = create_response(*view_response);
            return response;
        }
    }
    return "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>";
}

