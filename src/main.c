#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "string_help/string_help.h"
#include "json/json.h"
#include "request_type/request_type.h"
#include "views_and_paths/path.h"

#define PORT 8080 // Default port, TODO: make this configurable
#define MAX_LINES 100
#define RESPONSE_SIZE 300 
#define RESPONSE_SEND_SIZE 1024

void error(char *msg) {
    perror(msg);
    exit(1);
}

char *create_json_str(char **lines, int line_count) {
    char *json_str = malloc(1000 * sizeof(char));
    if (json_str == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }

    int found_json = 0;

    for (int i = 0; i < line_count; i++) {
        if (startsWith("{", lines[i]) == 1) {
            found_json = 1;
        }
        if (found_json == 1) {
            char *line = strdup(lines[i]);
            line = strip(line);
            json_str = realloc(json_str, strlen(json_str) + strlen(line) + 1);
            strcat(json_str, line);
        }
        free(lines[i]);
    }

    return json_str;
}

// Function to transfer the form string to a struct that is usable
data_t *create_form(char **lines, char *boundary, char* boundaryend, int line_count, int *form_data_size) {
    data_t *form_array = malloc(100 * sizeof(data_t)); // Allocate memory for the form data
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
            if (strcmp(value, "") == 0 || strcmp(name, "") == 0){
                free(name);
                free(value);
                continue;
            }
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

    //clear the console
    printf("\033[H\033[J");

    while (1) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &address)) < 0) {
            error("accept failed");
        }

        read(new_socket, buffer, 30000); // Read the request from the client

        // printf("%s\n", buffer); // Print the request

        int line_count;
        char **lines = split(buffer, "\n", &line_count); // Split the request into lines
        lines = realloc(lines, line_count * sizeof(char *)); // Reallocate memory for the lines

        

        struct request req;
        memset(&req, 0, sizeof(req)); // Initialize the request structure

        if (lines != NULL) {
            for (int i = 0; i < line_count - 1; i++) {
                if (i == 0) { // For the first line, get the method, path and version, since they never change position
                    int word_count;
                    char **words = split(lines[i], " ", &word_count);
                    if (words != NULL && word_count >= 3) {
                        req.method = realloc(req.method, strlen(words[0]) + 1);                        req.path = realloc(req.path, strlen(words[1]) + 1);
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
                        req.content_type = realloc(req.content_type, strlen(words[1]) + 1);
                        req.content_type = strdup(words[1]);
                        req.content_type[strlen(req.content_type) - 1] = '\0';
                        if(strcmp(req.content_type, "multipart/form-data") == 0) {
                            char *boundary = strdup("--"); // Boundary tag in the form data has 2 more - than it has when its in the content-type
                            char **boundarycat_split = split(lines[i], "=", &word_count); // Split the line to get the boundary tag
                            char *boundarycat = strdup(boundarycat_split[1]); // Get the boundary tag
                            boundary = realloc(boundary, strlen(boundary) + strlen(boundarycat) + 1); // Reallocate memory for the boundary
                            strcat(boundary, boundarycat); // Concatenate the boundary and the boundarycat

                            char *boundaryendcat = strdup("--"); // Boundary end tag has 2 - in the end of the tag
                            char *preboundaryendcat = strdup("--"); // Same reason as above
                            char *boundaryend = strdup(boundarycat_split[1]); // Get the boundary tag
                            boundaryend[strlen(boundaryend) - 1] = '\0'; // Remove the \r from the end of the boundaryend and replace it with \0
                            size_t len = strlen(boundaryend); // Get the length of the boundaryend
                            boundaryend = realloc(boundaryend, strlen(boundaryend) + strlen(boundaryendcat) + 1); // Reallocate memory for the boundaryend
                            strcat(boundaryend, boundaryendcat); // Concatenate the boundaryend and the boundaryendcat
                            boundary = realloc(boundary, strlen(boundary) + strlen(preboundaryendcat) + 1); // Reallocate memory for the boundary
                            strcat(preboundaryendcat, boundaryend); // Concatenate the preboundaryendcat and the boundaryend

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
                            data_t *form_data = create_form(&lines[i + 1], boundary, preboundaryendcat, line_count, &form_data_size); // Create the form data
                            req.data_size = form_data_size; // Set the form data size

                            if (form_data == NULL) {
                                free(boundary);
                                for (int j = 0; j < word_count; j++) {
                                    free(words[j]);
                                }
                                free(words);
                                continue;
                            }

                            // Set the data from the form data to the request
                            for (int j = 0; j < req.data_size; j++) {
                                req.data_union.form_data[j].key = form_data[j].key;
                                req.data_union.form_data[j].value = form_data[j].value;
                            }
                            free(form_data); // Free the form data
                            free(boundary); // Free the boundary

                            // Free the words array
                            for (int j = 0; j < word_count; j++) {
                                free(words[j]);
                            }
                            free(words);
                            break;
                        } else if (strcmp(req.content_type, "application/x-www-form-urlencoded") == 0) {
                            int word_count;
                            char **words = split(lines[i], " ", &word_count);
                            if (words != NULL && word_count >= 2) {
                                int length_split_count;
                                char *content_length = strdup(lines[i + 1]);
                                char **content_length_split = split(content_length, " ", &length_split_count);
                                req.content_length = atoi(content_length_split[1]);
                                free(words);
                                free(content_length);
                                free(content_length_split);
                                int data_size;
                                char **data = split(lines[i + 3], "&", &data_size);
                                for (int j = 0; j < data_size; j++) {
                                    int key_value_count;
                                    char **key_value = split(data[j], "=", &key_value_count);
                                    req.data_union.form_data[j].key = strdup(key_value[0]);
                                    req.data_union.form_data[j].value = strdup(key_value[1]);
                                    free(key_value);
                                }
                                req.data_size = data_size;
                                free(data);
                            }
                        } else if (strcmp(req.content_type, "application/json") == 0) {
                            int word_count;
                            char **words = split(lines[i], " ", &word_count);
                            if (words != NULL && word_count >= 2) {
                                for (size_t i = 0; i < line_count; i++)
                                {
                                    if (startsWith("{", lines[i]) == 1)
                                    {
                                        char *json_str = create_json_str(lines, line_count);
                                        lexer_t *lexer = create_lexer(json_str);
                                        json_value_t *value = parse_value(lexer);
                                        req.data_union.json_data = value->value.object;
                                        req.data_size = 1;
                                        break;
                                    }
                                }
                            }
                            for (int j = 0; j < word_count; j++) {
                                free(words[j]);
                            }
                            free(words);
                            break;
                        } else {
                        }
                    }
                }
                if(req.content_type == NULL) {
                    free(lines[i]);
                } else if (strcmp(req.content_type, "application/json") != 0){
                    free(lines[i]);
                }
            }
            free(lines); // Free the array of lines
            //clear the buffer
            memset(buffer, 0, sizeof(buffer));
        }

        char *response = malloc(RESPONSE_SEND_SIZE * sizeof(char));

        if (response == NULL) {
            perror("Failed to allocate memory");
            return 1;
        }

        if(includes(req.path, "?")){
            char *current_path = strdup(req.path);
            char **path_split = split(current_path, "?", &line_count);
            req.path = strdup(path_split[0]);
            char *query = strdup(path_split[1]);
            int query_count;
            char **query_split = split(query, "&", &query_count);
            int param_count = 0;
            for (int i = 0; i < query_count; i++) {
                char **key_value = split(query_split[i], "=", &line_count);
                req.data_union.form_data[req.data_size + i].key = strdup(key_value[0]);
                req.data_union.form_data[req.data_size + i].value = strdup(key_value[1]);
                param_count++;
            }
            req.data_size += param_count;
        }

        response = find_view(&req); // Find the view that should be used

        // Print the request
        printf("Method: %s\n", req.method);
        printf("Path: %s\n", req.path);
        printf("Version: %s\n", req.version);
        printf("Content-Type: %s\n", req.content_type);
        printf("Content-Length: %d\n", req.content_length);
        printf("Data: \n");
        if (req.content_type != NULL){
            if (req.data_size == 0) {
                printf("    No data\n");
            } else if (req.data_size > 0 && (strcmp(req.content_type, "multipart/form-data") == 0 || strcmp(req.content_type, "application/x-www-form-urlencoded") == 0)) {
                for (int i = 0; i < req.data_size; i++) {
                    printf("    Key: %s\n", req.data_union.form_data[i].key);
                    printf("    Value: %s\n", req.data_union.form_data[i].value);
                }
            } else if (req.data_size > 0 && strcmp(req.content_type, "application/json") == 0) {
                json_pair_t *current_pair = req.data_union.json_data->head;
                while (current_pair != NULL)
                {
                    printf("    Key: %s\n", current_pair->key);
                    printf("    Value: %s\n", current_pair->value->value.string);
                    current_pair = current_pair->next;
                }
            }
        } else if(req.data_size > 0) {
            for (int i = 0; i < req.data_size; i++) {
                printf("    Key: %s\n", req.data_union.form_data[i].key);
                printf("    Value: %s\n", req.data_union.form_data[i].value);
            }
        }

        write(new_socket, response, strlen(response));
        printf("response sent\n");

        close(new_socket); // Close the socket

        // Free req fields after use
        free(req.method);
        free(req.path);
        free(req.version);
        free(req.content_type);
        for (int i = 0; i < 100; i++) {
            if (req.data_union.form_data[i].key != NULL) {
                free(req.data_union.form_data[i].key);
            }
            if (req.data_union.form_data[i].value != NULL) {
                free(req.data_union.form_data[i].value);
            }
        }
    }

    return 0;
}