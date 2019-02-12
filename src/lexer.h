#pragma once

#include "tokens.h"
#include <lab/vector.h>

typedef struct lab_lexer_iter {
    size_t i;
    int line;
    int column;
} lab_lexer_iter_t;

void lab_lexer_iter_next(const char* code, lab_lexer_iter_t* iter);
void lab_lexer_iter_prev(const char* code, lab_lexer_iter_t* iter);

const char* lab_token_to_string_lookup[47];

typedef struct lab_lexer_token {
    lab_tokens_e_t type;
    const char* data;
    size_t data_len;
    int line;
    int column;
} lab_lexer_token_t;

typedef struct lab_lexer_token_container {
    lab_vec_t tokens;
    const char* code;
} lab_lexer_token_container_t;

bool lab_lexer_token_container_init(lab_lexer_token_container_t* container, size_t init_size);
void lab_lexer_token_container_free(lab_lexer_token_container_t* container);

bool lab_lexer_token_container_append(lab_lexer_token_container_t* container,
                                      size_t code_size,
                                      lab_lexer_iter_t iter,
                                      lab_tokens_e_t type,
                                      const char* data,
                                      size_t data_len);

void lab_lexer_token_container_print(lab_lexer_token_container_t* container);

bool lab_lexer_lex(lab_lexer_token_container_t* container, const char* code);