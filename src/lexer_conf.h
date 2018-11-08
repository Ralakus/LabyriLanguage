#pragma once

#include <lab/logger.h>
#include "lexer.h"
#include "tokens.h"

extern char* tok_to_string(lab_tokens_e tok);

extern lab_lexer_token_t alpha_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data);

extern lab_lexer_token_t whitespace_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data);

extern lab_lexer_token_t numeric_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data);

extern lab_lexer_token_t symbol_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data);

extern lab_lexer_token_t operator_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data);

extern lab_lexer_token_t string_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data);

/*
    TODO: Find a way to make the lexer take an a null termination as a rule
*/
extern lab_lexer_token_t eof_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data);

extern int lab_custom_lexer_lex(lab_lexer_token_container_t* tokens, const char* code, size_t code_len, void* user_data);