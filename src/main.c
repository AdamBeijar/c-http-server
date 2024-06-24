#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "string_help.h"

#define PORT 8080
#define RESPONSE \
    "HTTP/1.1 200 OK\n" \
    "Content-Type: text/plain\n"\
    "Content-Length: 12\n\n"\
    "Hello world!"
#define MAX_LINES 100
#define RESPONSE_SIZE 300 

void error(char *msg) {
    perror(msg);
    exit(1);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
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

        read(new_socket, buffer, 30000);
        printf("%s\n", buffer);

        int line_count;
        char **lines = split(buffer, "\n", &line_count);

        struct data
        {
            char *name;
            char *value;
        };
        

        struct request {
            char *method;
            char *path;
            char *version;
            char *content_type;
            int content_length;
            struct data *data[100];
        };


        struct request req;

        if (lines != NULL) {
            for (int i = 0; i < line_count - 1; i++) {
                if (i == 0) {
                    int word_count;
                    char **words = split(lines[i], " ", &word_count);
                    if (words != NULL && word_count >= 3) {
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
                if (startsWith("Content-Type:", lines[i]) == 1) {
                    int word_count;
                    char **words = split(lines[i], " ", &word_count);
                    printf("word count: %d\n", word_count);
                    printf("words[1]: %s\n", words[1]);
                    printf("Line: %s\n", lines[i]);
                    if (words != NULL && word_count >= 2) {
                        req.content_type = strdup(words[1]);
                        // Free the words array
                        for (int j = 0; j < word_count; j++) {
                            free(words[j]);
                        }
                        free(words);
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

        write(new_socket, RESPONSE, strlen(RESPONSE));
        printf("response sent\n");

        close(new_socket);

        // Free req fields after use
        free(req.method);
        free(req.path);
        free(req.version);
    }

    return 0;
}
