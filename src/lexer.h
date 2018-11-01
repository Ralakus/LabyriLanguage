#pragma once

#include <logger.h>
#include <lexer.h>

typedef enum tokens_e {
    tok_nil,

    tok_whitespace_space,
    tok_whitespace_tab,
    tok_whitespace_return,
    tok_whitespace_newline,

    tok_identifier,
    tok_number,

    tok_char,
    tok_string,

    tok_lparen,
    tok_rparen,
    tok_lbracket,
    tok_rbracket,
    tok_lcurley,
    tok_rcurley,

    tok_comma,
    tok_colon,
    tok_semicolon,

    tok_func,
    tok_let,
    tok_return,

    tok_operator_plus,
    tok_operator_minus,
    tok_operator_mul,
    tok_operator_div,
    tok_operator_equals,
    tok_operator_xor,
    tok_operator_and,
    tok_operator_lesst,
    tok_operator_greatert,
    tok_operator_or,
    tok_operator_not,

    tok_eof,

}   tokens_e;

extern char* tok_to_string(tokens_e tok);

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

extern int custom_lexer_lex(lab_lexer_token_container_t* tokens, const char* code, size_t code_len, void* user_data);