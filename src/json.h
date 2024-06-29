#ifndef JSON_H
#define JSON_H

#include <stddef.h>

typedef enum {
    JSON_TYPE_NULL,
    JSON_TYPE_BOOL,
    JSON_TYPE_NUMBER,
    JSON_TYPE_STRING,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT
} json_type_t;

typedef struct json_value_t {
    json_type_t type;
    union {
        char *string;
        double number;
        int boolean;
        struct json_object_t *object;
        struct json_array_t *array;
    } value;
} json_value_t;

typedef struct json_pair_t {
    char *key;
    struct json_value_t *value;
    struct json_pair_t *next;
} json_pair_t;

typedef struct json_object_t {
    struct json_pair_t *head;
} json_object_t;

typedef struct json_array_t {
    json_value_t *values;
    size_t size;

} json_array_t;

typedef enum {
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_LBRACE, // {
    TOKEN_RBRACE, // }
    TOKEN_LBRACKET, // [
    TOKEN_RBRACKET, // ]
    TOKEN_COMMA, // ,
    TOKEN_COLON, // :
    TOKEN_ERROR,
    TOKEN_EOF
} token_type_t;

typedef struct token_t {
    token_type_t type;
    char *value;
} token_t;

typedef struct lexer_t {
    char *input;
    size_t len;
    size_t pos;
} lexer_t;

token_t next_token(lexer_t *lexer);

lexer_t *create_lexer(char *input);

typedef struct {
    token_t *tokens;
    size_t size;
    size_t capacity;
} token_list_t;

#endif

#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include "json.h"

json_value_t *parse_value(lexer_t *lexer);

#endif