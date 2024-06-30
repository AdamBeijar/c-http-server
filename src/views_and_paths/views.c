#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../request_type/request_type.h"
#include "path.h"
#include "../string_help/string_help.h"


// Example view function for the index page
view_t *index_view(struct request *req) {
    char *name = NULL;

    // if the data has no size, set name to "World"
    if (req->data_size == 0) {
        name = malloc(6);
        name = "World";
    } else {
        for (int i = 0; i < req->data_size; i++) { // Loop through the form data
            if (strcmp(req->data_union.form_data[i].key, "name") == 0) { // If the key is "name", set name to the value
                name = malloc(strlen(req->data_union.form_data[i].value) + 1);
                name = req->data_union.form_data[i].value;
            }
        }
    }

    if (name == NULL) { // if name is still NULL, set it to "World"
        name = malloc(6);
        name = "World";
    }

    int status_code = 200;
    char *content_type = "text/html";
    char *content = malloc(strlen(name) + 1);

    int line_count = 0;
    char **html_file = open_html_file("templates/index.html", &line_count); // Open the html file

    for (int i = 0; i < line_count; i++) {
        if (includes(html_file[i], "{{ name }}") == 1) { // If the line contains "{{ name }}", replace it with the name
            char *new_content = replace(html_file[i], "{{ name }}", name);
            html_file[i] = new_content;
        } 
    }

    content = build_html_response(html_file, &line_count); // Build the html from the modified html file

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

view_t *contact_view(struct request *req) {
    int status_code = 200;
    char *content_type = "text/html";
    char email[] = "adam.beijar@gmail.com";
    char *name;

    if (req->data_size == 0) {
        name = malloc(6);
        name = "World";
    } else {
        for (int i = 0; i < req->data_size; i++) {
            if (strcmp(req->data_union.form_data[i].key, "name") == 0) {
                name = malloc(strlen(req->data_union.form_data[i].value) + 1);
                name = req->data_union.form_data[i].value;
            }
        }
    }

    int line_count = 0;
    char **html_file = open_html_file("templates/contact.html", &line_count);


    for (int i = 0; i < line_count; i++) {
        if (includes(html_file[i], "{{ name }}") == 1) {
            char *new_content = replace(html_file[i], "{{ name }}", name);
            html_file[i] = new_content;
        } 
        if (includes(html_file[i], "{{ email }}") == 1) {
            char *new_content = replace(html_file[i], "{{ email }}", email);
            html_file[i] = new_content;
        }
    }

    char *content = build_html_response(html_file, &line_count);

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

view_t *index_post_view(struct request *req) {
    char *name;
    if (req->data_size == 0) {
        name = malloc(6);
        name = "World";
    } else {
        for (int i = 0; i < req->data_size; i++) {
            if (strcmp(req->data_union.form_data[i].key, "name") == 0) {
                name = malloc(strlen(req->data_union.form_data[i].value) + 1);
                name = req->data_union.form_data[i].value;
            }
        }
    }
    int status_code = 200;
    char *content_type = "application/json";
    char *content = malloc(strlen(name) + 1);
    sprintf(content, "{\"message\": \"Hello, %s\"}", name);
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