#include "../json/json.h"

typedef struct data_struct { // Struct to hold form data
    char *key;
    char *value;
} data_t;

typedef struct request {
    char *method;
    char *path;
    char *version;
    char *content_type;
    int content_length;
    union
    {
        data_t form_data[100];
        json_object_t *json_data;
    } data_union;
    
    int data_size;
} request_t;