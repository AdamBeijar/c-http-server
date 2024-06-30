#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "path.h"
#include "../string_help/string_help.h"

view_t *handle_not_found() {
    printf("File Not Found\n");
    view_t *view = malloc(sizeof(view_t));

    if (view == NULL) {
        perror("malloc");
        exit(1);
    }

    view->status_code = 404;
    view->content_type = "text/html";
    view->content = "<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</hh1></body></html>";

    return view;
}

view_t *handle_static(char *path, char **static_path_split, int static_path_count) {
    char *static_type = static_path_split[1];

    if (strcmp(static_type, "css") == 0) {
        removeSubString(path, "/", 1);

        FILE *css_file = fopen(path, "r");

        if (css_file == NULL) {
            view_t *view = handle_not_found();
            return view;
        }   

        view_t *view = malloc(sizeof(view_t));

        if (view == NULL) {
            perror("malloc");
            exit(1);
        }

        if (css_file == NULL) {
            perror("fopen");
            exit(1);
        }

        fseek(css_file, 0, SEEK_END);
        long css_file_size = ftell(css_file);
        fseek(css_file, 0, SEEK_SET);

        char *css_content = malloc(css_file_size + 1);
        if (css_content == NULL) {
            perror("malloc");
            exit(1);
        }

        fread(css_content, 1, css_file_size, css_file);
        fclose(css_file);

        view->status_code = 200;
        view->content_type = "text/css";
        view->content = css_content;

        return view;
    } else if (strcmp(static_type, "js") == 0 || strcmp(static_type, "javascript") == 0) {
        removeSubString(path, "/", 1);


        FILE *js_file = fopen(path, "r");

        if (js_file == NULL) {
            view_t *view = handle_not_found();
            return view;
        }

        view_t *view = malloc(sizeof(view_t));

        if (view == NULL) {
            perror("malloc");
            exit(1);
        }

        if (js_file == NULL) {
            perror("fopen");
            exit(1);
        }

        fseek(js_file, 0, SEEK_END);
        long js_file_size = ftell(js_file);
        fseek(js_file, 0, SEEK_SET);

        char *js_content = malloc(js_file_size + 1);
        if (js_content == NULL) {
            perror("malloc");
            exit(1);
        }

        fread(js_content, 1, js_file_size, js_file);
        fclose(js_file);

        view->status_code = 200;
        view->content_type = "application/javascript";
        view->content = js_content;

        return view;
    }
    

    view_t *view = malloc(sizeof(view_t));

    if (view == NULL) {
        perror("malloc");
        exit(1);
    }

    view->status_code = 404;
    view->content_type = "text/html";
    view->content = "<!DOCTYPE html><html><head><title>404 Not Found</title></head><body><h1>404 Not Found</hh1></body></html>";

    return view;
}