#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
    const char *input;
    size_t pos;
    size_t len;
} lexer_t;

lexer_t *create_lexer(const char *input) {
    lexer_t *lexer;
    lexer->input = input;
    lexer->pos = 0;
    lexer->len = strlen(input);
    return lexer;
}

char next_char(lexer_t *lexer) {
    if (lexer->pos >= lexer->len) {
        return '\0';
    }
    return lexer->input[lexer->pos++];
}

char peek_char(lexer_t *lexer) {
    if (lexer->pos >= lexer->len) {
        return '\0';
    }
    return lexer->input[lexer->pos];
}

void skip_whitespace(lexer_t *lexer) {
    char c;
    while ((c = peek_char(lexer)) != '\0') {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            next_char(lexer);
        } else {
            break;
        }
    }
}

token_t next_token(lexer_t *lexer) {
    skip_whitespace(lexer);

    token_t token;
    token.value = NULL;

    char c = next_char(lexer);

    switch (c)
    {
        case '\0': token.type = TOKEN_EOF; break;
        case '{': token.type = TOKEN_LBRACE; break;
        case '}': token.type = TOKEN_RBRACE; break;
        case '[': token.type = TOKEN_LBRACKET; break;
        case ']': token.type = TOKEN_RBRACKET; break;
        case ',': token.type = TOKEN_COMMA; break;
        case ':': token.type = TOKEN_COLON; break;
        case 't': {
            if (peek_char(lexer) == 'r' && next_char(lexer) == 'u' &&
                peek_char(lexer) == 'e' && next_char(lexer) == '\0') {
                token.type = TOKEN_TRUE;
            } else {
                token.type = TOKEN_ERROR;
            }
            break;
        }
        case 'f': {
            if (peek_char(lexer) == 'a' && next_char(lexer) == 'l' &&
                peek_char(lexer) == 's' && next_char(lexer) == 'e' &&
                peek_char(lexer) == '\0') {
                token.type = TOKEN_FALSE;
            } else {
                token.type = TOKEN_ERROR;
            }
            break;
        }
        case 'n': {
            if (peek_char(lexer) == 'u' && next_char(lexer) == 'l' &&
                peek_char(lexer) == 'l' && next_char(lexer) == '\0') {
                token.type = TOKEN_NULL;
            } else {
                token.type = TOKEN_ERROR;
            }
            break;
        }
        case '"': {
            size_t size = 0;
            char *buffer = malloc(1);
            while (1) {
                c = next_char(lexer);
                if (c == '\0') {
                    token.type = TOKEN_ERROR;
                    break;
                } else if (c == '"') {
                    buffer[size] = '\0';
                    token.type = TOKEN_STRING;
                    token.value = buffer;
                    break;
                } else {
                    buffer = realloc(buffer, size + 1);
                    buffer[size++] = c;
                }
            }
            break;
        }
        default: {
            if (c == '-' || (c >= '0' && c <= '9')) {
                size_t size = 1;
                char *buffer = malloc(1);
                buffer[0] = c;
                while (1) {
                    c = peek_char(lexer);
                    if (c == '\0' || c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',' || c == '}' || c == ']') {
                        buffer[size] = '\0';
                        token.type = TOKEN_NUMBER;
                        token.value = buffer;
                        break;
                    } else {
                        buffer = realloc(buffer, size + 1);
                        buffer[size++] = next_char(lexer);
                    }
                }
            } else {
                token.type = TOKEN_ERROR;
            }
            break;
        }
    }

    return token;
}