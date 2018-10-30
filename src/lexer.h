#pragma once

#include <stddef.h>

typedef struct lab_lexer_token_t {
    int id;
    char* data;
} lab_lexer_token_t;
// ^ Contains the id of a token and any data it might need, the data is freed when lab_lexer_token_container_free is called

typedef struct lab_lexer_token_container_t {
    size_t alloc_count;
    size_t count;
    lab_lexer_token_t* tokens;
} lab_lexer_token_container_t;
// ^ holds tokens in a dynamic array

typedef struct lab_lexer_iterator_t {
    size_t iter;
    size_t line;
    size_t column;
} lab_lexer_iterator_t;

typedef struct lab_lexer_rules_t     lab_lexer_rules_t;
//             ^ contains lexer rules, id est what to look for and callback
typedef lab_lexer_token_t  (*lab_lexer_callback_t)(const char* code, lab_lexer_iterator_t* iter, size_t max_length, void* user_data);
//             ^ returns token type by int id

extern lab_lexer_rules_t* lab_lexer_rules_new();
//         ^ intializes new rules struct
extern int lab_lexer_rules_free(lab_lexer_rules_t* rules);
//         ^ frees rules struct

extern int lab_lexer_token_container_init(lab_lexer_token_container_t* container);
//         ^ inits token container
extern int lab_lexer_token_container_free(lab_lexer_token_container_t* container);
//         ^ frees token container
extern int lab_token_container_append(lab_lexer_token_container_t* container, lab_lexer_token_t token, const lab_lexer_iterator_t* pos, size_t max_code_len);
//         ^ appends a token to a token container

extern lab_lexer_token_t lab_lexer_token_make(int id, char* data);
//                       ^ makes new token with id and data

extern int lab_lexer_add_rule(lab_lexer_rules_t*     rules, const char* rule, lab_lexer_callback_t     callback);
//  ^ adds rule to rule container
/*
    A rule is a string like "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm".
    What it means is whenever the lexer encounters one of those characters, it calls the callback function
*/
extern int lab_lexer_lex     (lab_lexer_token_container_t* lexer, const char* code, size_t code_len, const lab_lexer_rules_t* rules, void* user_data);
//  ^ runs lexer with rules
extern void lab_lexer_iter_next(const char* code, lab_lexer_iterator_t* iter);
//         ^ iterates to next character in code, automatically changes lines and columns