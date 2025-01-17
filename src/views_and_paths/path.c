#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../request_type/request_type.h"
#include "views.h"
#include "../string_help/string_help.h"
#include "static.h"

typedef struct path_t {
    char *path;
    char *method;
    view_t *(*view)(struct request *req);
} path_t;


// Creates the respone from the view_t struct
char *create_response(view_t view) {
    char *response = malloc(strlen(view.content) + 100);

    if(response == NULL) {
        perror("malloc");
        exit(1);
    }

    sprintf(response, "HTTP/1.1 %d OK\nContent-Type: %s\n\n%s", view.status_code, view.content_type, view.content);
    return response;
}

// Find the view that corresponds to the path
char *find_view(request_t *req) {
    path_t paths[] = { // Here you add the paths and the corresponding views, with {"/path", "METHOD", view_function}
        {"/", "GET", index_view},
        {"/contact", "GET", contact_view},
        {"/", "POST", index_post_view}
    };
    char *path = req->path;
    size_t path_len = strlen(path);
    if (path[path_len - 1] == '/') { // If the path has a trailing slash, remove it
        path[path_len - 1] = '\0';
    }
    for (int i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        char *current_path = strdup(paths[i].path);
        size_t current_path_len = strlen(current_path);
        if (current_path[current_path_len - 1] == '/') {
            current_path[current_path_len - 1] = '\0';
        }
        if(startWithStr("/static/", path) == 1) { 
        /*
        If the path starts with "/static/", it is a static file.
        To get static files working properly, you need to create a folder called "static" in the root of your project. And after that place the static files in a directory with the correct name.
        So if you want to serve a css file, you need to place it in the "static/css" directory, and if you want to serve a js file, you need to place it in the "static/js" or "static/javascript" directory.
        Every other files should be placed in a directory with the same name as the first part of the content-type, e.g if you have a png, you need to place it in the "static/image" directory, and if you have a json file, you need to place it in the "static/application" directory.
        */
            int static_path_count = 0;
            char **static_path_split = split(path, "/", &static_path_count);
            for (int i = 0; i < static_path_count; i++) {
                view_t *static_response = handle_static(path, static_path_split, static_path_count);
                char *response = create_response(*static_response);
                return response;
            }
        }
        if (strcmp(current_path, path) == 0 && strcmp(paths[i].method, req->method) == 0) { // If the path and method match, return the view
            view_t *view_response = paths[i].view(req); // Call the view function
            char *response = create_response(*view_response);
            return response;
        }
    }
    return "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</h1></body></html>";
}

// Open the html file and return it as an array of strings
char **open_html_file(char *html, int *line_count) {
    FILE *file = fopen(html, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    char **html_file = malloc(1000 * sizeof(char *));
    if (html_file == NULL) {
        perror("malloc");
        exit(1);
    }
    char *line = NULL;
    size_t len = 0;
    int i = 0;
    while (getline(&line, &len, file) != -1) {
        html_file[i] = malloc(strlen(line) + 1);
        if (html_file[i] == NULL) {
            perror("malloc");
            exit(1);
        }
        strcpy(html_file[i], line);
        i++;
    }
    *line_count = i;
    html_file[i] = NULL;
    fclose(file);
    return html_file;
}


// Build the html response from the array of strings
char *build_html_response(char **html, int *line_count){
    char *response;
    if(response == NULL) {
        perror("malloc");
        exit(1);
    }
    response = malloc(1024);
    for (int i = 0; i < *line_count; i++) {
        strcat(response, html[i]);
    }
    return response;
}