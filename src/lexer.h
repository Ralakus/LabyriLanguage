#pragma once

#include "tokens.h"
#include "lab/vector.h"

typedef struct lab_lexer_token {
    lab_tokens_e_t type;
    const char* data;
    size_t data_len;
    size_t line;
    size_t column;
} lab_lexer_token_t;

typedef struct lab_lexer_token_container {
    lab_vec_t tokens;
    const char* code;
} lab_lexer_token_container_t;

bool lab_lexer_token_container_init(lab_lexer_token_container_t* container);
void lab_lexer_token_container_free(lab_lexer_token_container_t* container);

bool lab_lexer_token_container_append(lab_lexer_token_container_t* container,
                                      size_t code_size,
                                      size_t iter,
                                      lab_tokens_e_t type,
                                      const char* data,
                                      size_t data_len,
                                      size_t line,
                                      size_t column);