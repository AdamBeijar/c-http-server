#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "string_help.h"

#define PORT 8080 // Default port, TODO: make this configurable
#define RESPONSE \
    "HTTP/1.1 200 OK\n" \
    "Content-Type: text/plain\n"\
    "Content-Length: 12\n\n"\
    "Hello world!" // Default response, TODO: make this configurable
#define MAX_LINES 100
#define RESPONSE_SIZE 300 

void error(char *msg) {
    perror(msg);
    exit(1);
}

struct data { // Struct to hold form data
    char *key;
    char *value;
};

// Function to transfer the form string to a struct that is usable
struct data *create_form(char **lines, char *boundary, char* boundaryend, int line_count, int *form_data_size) {
    struct data *form_array = malloc(100 * sizeof(struct data)); // Allocate memory for the form data
    if (form_array == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    if (boundaryend == NULL) {
        perror("realloc failed");
        free(form_array);
        return NULL;
    }

    int form_index = 0;
    for (int i = 0; i < line_count - 1; i++) {
        if (startsWith(boundaryend, lines[i]) == 1) { // If the line starts with the boundary end, en the loop and return the form data
            break;
        } else if (startsWith(boundary, lines[i]) == 1) { // If the line starts with the boundary, then it is a form data
            int word_count;
            char **name_split = split(lines[i+1], "=", &word_count); // Split the line to get the name
            if (name_split == NULL) {
                perror("split failed");
                continue;
            }
            
            char *name = strdup(name_split[1]); // Get the name

            removeAllSubString(name, "\""); // Remove the quotes from the name
            name[strlen(name) - 1] = '\0'; // Remove the \r from the end of the name and replace it with \0
            char *value = strdup(lines[i + 3]); // Get the value
            removeAllSubString(value, "\r\n"); // Remove the \r\n from the end of the value
            strip(value); // Strip the value
            form_array[form_index].key = strdup(name); 
            form_array[form_index].value = strdup(value);
            form_index++;
        }
    }

    *form_data_size = form_index; // Set the form data size
    free(boundaryend); // Free the boundary end
    return form_array;
}

// Function to count the number of form data in the form
int count_form_length(char **lines, char *boundary, int line_count) {
    int count = 0;
    for (int i = 0; i < line_count - 1; i++) {
        if (startsWith(boundary, lines[i]) == 1) {
            count++;
        }
    }
    return count - 1; // Return with count - 1 because the last boundary is not a form data
}

int main() {
    int server_fd, new_socket; // File descriptors
    struct sockaddr_in address; // Address struct
    char buffer[30000] = {0}; 

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("socket failed");
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        error("Setsockopt SO_REUSEADDR failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        error("bind failed");
    }

    if (listen(server_fd, 3) < 0) {
        error("listen failed");
    }

    while (1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &address)) < 0) {
            error("accept failed");
        }

        read(new_socket, buffer, 30000); // Read the request from the client

        int line_count;
        char **lines = split(buffer, "\n", &line_count); // Split the request into lines

        struct request {
            char *method;
            char *path;
            char *version;
            char *content_type;
            int content_length;
            struct data data[100];
            int data_size;
        };

        struct request req;
        memset(&req, 0, sizeof(req)); // Initialize the request structure

        if (lines != NULL) {
            for (int i = 0; i < line_count - 1; i++) {
                if (i == 0) { // For the first line, get the method, path and version, since they never change position
                    int word_count;
                    char **words = split(lines[i], " ", &word_count);
                    if (words != NULL && word_count >= 3) {
                        req.method = realloc(req.method, strlen(words[0]) + 1);
                        req.path = realloc(req.path, strlen(words[1]) + 1);
                        req.version = realloc(req.version, strlen(words[2]) + 1);
                        req.method = strdup(words[0]);
                        req.path = strdup(words[1]);
                        req.version = strdup(words[2]);


                        // Free the words array
                        for (int j = 0; j < word_count; j++) {
                            free(words[j]);
                        }
                    }
                    free(words);
                }
                if (startsWith("Content-Type:", lines[i]) == 1) { // If the line starts with Content-Type, then get the content type
                    int word_count;
                    char **words = split(lines[i], " ", &word_count);
                    if (words != NULL && word_count >= 2) {
                        req.content_type = strdup(words[1]);
                        req.content_type[strlen(req.content_type) - 1] = '\0';
                        if(strcmp(req.content_type, "multipart/form-data") == 0) {

                            char *boundary = strdup("--");
                            char **boundarycat_split = split(lines[i], "=", &word_count);
                            char *boundarycat = strdup(boundarycat_split[1]);
                            boundary = realloc(boundary, strlen(boundary) + strlen(boundarycat) + 1);

                            char *boundaryendcat = strdup("--");
                            char *preboundaryendcat = strdup("--");
                            strcat(boundary, boundarycat);
                            char *boundaryend = strdup(boundarycat_split[1]);
                            boundaryend[strlen(boundaryend) - 1] = '\0';
                            size_t len = strlen(boundaryend);
                            boundaryend = realloc(boundaryend, strlen(boundaryend) + strlen(boundaryendcat) + 1);
                            strcat(boundaryend, boundaryendcat);
                            boundary = realloc(boundary, strlen(boundary) + strlen(preboundaryendcat) + 1);
                            strcat(preboundaryendcat, boundaryend);

                            if (boundary == NULL || boundaryend == NULL) {
                                perror("realloc failed");
                                free(req.content_type);
                                for (int j = 0; j < word_count; j++) {
                                    free(words[j]);
                                }
                                free(words);
                                continue;
                            }
                            int form_data_size;
                            struct data *form_data = create_form(&lines[i + 1], boundary, preboundaryendcat, line_count, &form_data_size);
                            req.data_size = form_data_size;
                            if (form_data == NULL) {
                                free(boundary);
                                for (int j = 0; j < word_count; j++) {
                                    free(words[j]);
                                }
                                free(words);
                                continue;
                            }

                            for (int j = 0; j < req.data_size; j++) {
                                req.data[j].key = form_data[j].key;
                                req.data[j].value = form_data[j].value;
                            }
                            free(form_data);
                            free(boundary);

                            // Free the words array
                            for (int j = 0; j < word_count; j++) {
                                free(words[j]);
                            }
                            free(words);
                        }
                    }
                }
                free(lines[i]); // Free each line after use
            }
            free(lines); // Free the array of lines
        }

        printf("Method: %s\n", req.method);
        printf("Path: %s\n", req.path);
        printf("Version: %s\n", req.version);
        printf("Content-Type: %s\n", req.content_type);
        printf("Content-Length: %d\n", req.content_length);
        printf("Data: \n");
        for (int i = 0; i < req.data_size; i++) {
            printf("    Name: %s\n", req.data[i].key);
            printf("    Value: %s\n", req.data[i].value);
        }

        write(new_socket, RESPONSE, strlen(RESPONSE));
        printf("response sent\n");

        close(new_socket);

        // Free req fields after use
        free(req.method);
        free(req.path);
        free(req.version);
        free(req.content_type);
        for (int i = 0; i < 100; i++) {
            if (req.data[i].key != NULL) {
                free(req.data[i].key);
            }
            if (req.data[i].value != NULL) {
                free(req.data[i].value);
            }
        }
    }

    return 0;
}