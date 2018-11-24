#pragma once

#include "ast.h"
#include "lexer.h"

typedef struct lab_parser_token_container_t {

    lab_lexer_token_container_t* tokens;
    size_t cur;

} lab_parser_token_container_t;

extern bool lab_parser_token_container_init(lab_parser_token_container_t* container, lab_lexer_token_container_t* tokens);