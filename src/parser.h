#pragma once

#include "lexer.h"
#include "vm.h"

typedef struct lab_parser {
    lab_lexer_token_container_t* container;
    lab_tokens_e_t* current;
    size_t index;
    lab_vm_bytecode_t* bytecode;
} lab_parser_t;

void lab_parser_init(lab_parser_t* parser);
void lab_parser_free(lab_parser_t* parser);