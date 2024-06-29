#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

json_value_t *parse_value(lexer_t *lexer);

json_value_t *parse_array(lexer_t *lexer){
    json_array_t *array = malloc(sizeof(json_array_t));
    array->values = NULL;
    array->size = 0;

    while (1)
    {
        json_value_t *value = parse_value(lexer);

        array->values = realloc(array->values, sizeof(json_value_t) * (array->size + 1));
        array->values[array->size] = *value;
        array->size++;

        token_t comma_or_end = next_token(lexer);

        if (comma_or_end.type == TOKEN_RBRACKET)
        {
            break;
        }
        else if (comma_or_end.type != TOKEN_COMMA)
        {
            fprintf(stderr, "Expected comma or end of array, got %d\n", comma_or_end.type);
            exit(1);
        }
    }

    json_value_t *value = malloc(sizeof(json_value_t));
    value->type = JSON_TYPE_ARRAY;
    value->value.array = array;
    return value;
}

json_value_t *parse_object(lexer_t *lexer){
    json_object_t *object = malloc(sizeof(json_object_t));
    object->head = NULL;
    json_pair_t *current_pair = NULL;

    while (1)
    {
        token_t key_token = next_token(lexer);
        if (key_token.type != TOKEN_STRING)
        {
            fprintf(stderr, "Expected string, got %d\n", key_token.type);
            exit(1);
        }

        char *key = malloc(strlen(key_token.value) + 1);
        strcpy(key, key_token.value);
        
        token_t colon_token = next_token(lexer);

        if (colon_token.type != TOKEN_COLON)
        {
            fprintf(stderr, "Expected colon, got %d\n", colon_token.type);
            exit(1);
        }

        json_value_t *value = parse_value(lexer);

        json_pair_t *pair = malloc(sizeof(json_pair_t));
        pair->key = key;
        pair->value = value;
        pair->next = NULL;

        if (current_pair == NULL)
        {
            object->head = pair;
            current_pair = pair;
        }
        else
        {
            current_pair->next = pair;
            current_pair = pair;
        }

        token_t comma_or_end = next_token(lexer);

        if (comma_or_end.type == TOKEN_RBRACE)
        {
            break;
        }
        else if (comma_or_end.type != TOKEN_COMMA)
        {
            fprintf(stderr, "Expected comma or end of object, got %d\n", comma_or_end.type);
            exit(1);
        }
    }

    json_value_t *value = malloc(sizeof(json_value_t));
    value->type = JSON_TYPE_OBJECT;
    value->value.object = object;
    return value;
}

json_value_t *parse_value(lexer_t *lexer){
    token_t token = next_token(lexer);
    
    switch (token.type)
    {
        case TOKEN_LBRACE: return parse_object(lexer);
        case TOKEN_LBRACKET: return parse_array(lexer);
        case TOKEN_STRING:
        {
            json_value_t *value = malloc(sizeof(json_value_t));
            value->type = JSON_TYPE_STRING;
            value->value.string = malloc(strlen(token.value) + 1);
            strcpy(value->value.string, token.value);
            return value;
        }
        case TOKEN_NUMBER:
        {
            json_value_t *value = malloc(sizeof(json_value_t));
            value->type = JSON_TYPE_NUMBER;
            value->value.number = atof(token.value);
            return value;
        }
        case TOKEN_TRUE:
        {
            json_value_t *value = malloc(sizeof(json_value_t));
            value->type = JSON_TYPE_BOOL;
            value->value.boolean = 1;
            return value;
        }
        case TOKEN_FALSE:
        {
            json_value_t *value = malloc(sizeof(json_value_t));
            value->type = JSON_TYPE_BOOL;
            value->value.boolean = 0;
            return value;
        }
        case TOKEN_NULL:
        {
            json_value_t *value = malloc(sizeof(json_value_t));
            value->type = JSON_TYPE_NULL;
            return value;
        }
        default:
            fprintf(stderr, "Unexpected token type %d\n", token.type);
            exit(1);
    }

    return NULL;
}