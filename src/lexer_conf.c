#include "lexer_conf.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define TOK_TO_STRING_TEMPLATE(token, string) case token: {                                                                 \
                                                  buffer = (char*)malloc(strlen(string)+1);                                 \
                                                  if(buffer==NULL) {                                                        \
                                                      lab_errorln("Failed to allocate buffer for function "#string"!");     \
                                                      return NULL;                                                          \
                                                  }                                                                         \
                                                  buffer[strlen(string)] = '\0';                                            \
                                                  strcpy(buffer, string);                                                   \
                                                  break;                                                                    \
                                              }

char* tok_to_string(lab_tokens_e tok) {
    char* buffer = NULL;
    switch(tok) {
        TOK_TO_STRING_TEMPLATE(lab_tok_nil, "nil")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_space, "whitespace space")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_tab, "whitespace tab")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_newline, "whitespace newline")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_return, "whitespace return")
        TOK_TO_STRING_TEMPLATE(lab_tok_identifier, "identifier")
        TOK_TO_STRING_TEMPLATE(lab_tok_number, "number")
        TOK_TO_STRING_TEMPLATE(lab_tok_char, "char")
        TOK_TO_STRING_TEMPLATE(lab_tok_string, "string")
        TOK_TO_STRING_TEMPLATE(lab_tok_lparen, "left paren")
        TOK_TO_STRING_TEMPLATE(lab_tok_rparen, "right paren")
        TOK_TO_STRING_TEMPLATE(lab_tok_lbracket, "left bracket")
        TOK_TO_STRING_TEMPLATE(lab_tok_rbracket, "right bracket")
        TOK_TO_STRING_TEMPLATE(lab_tok_lcurley, "left curley")
        TOK_TO_STRING_TEMPLATE(lab_tok_rcurley, "right curley")
        TOK_TO_STRING_TEMPLATE(lab_tok_comma, "comma")
        TOK_TO_STRING_TEMPLATE(lab_tok_colon, "colon")
        TOK_TO_STRING_TEMPLATE(lab_tok_semicolon, "semicolon")
        TOK_TO_STRING_TEMPLATE(lab_tok_comment, "comment")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_func, "function")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_let, "let")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_return, "return")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_as, "as")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_plus, "operator plus")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_minus, "operator minus")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_mul, "operator multiply")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_div, "operator divide")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_equals, "operator equals")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_xor, "operator xor")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_and, "operator and")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_lesst, "operator less than")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_greatert, "operator greater than")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_or, "operator or")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_not, "operator not")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_bitshiftl, "left bitshift")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_bitshiftr, "right bitshift")
        TOK_TO_STRING_TEMPLATE(lab_tok_eof, "end of file")
        default: {
            break;
        }
    }
    return buffer;
}

extern bool              alpha_callback_rule(char c) { return (isalpha(c) > 0); }
extern lab_lexer_token_t alpha_callback(const lab_vec_t* code,
                                        lab_lexer_iterator_t* iter, 
                                        lab_lexer_token_container_t* tokens, 
                                        void* user_data);

extern bool              whitespace_callback_rule(char c) { return (isspace(c) > 0); }
extern lab_lexer_token_t whitespace_callback(const lab_vec_t* code,
                                             lab_lexer_iterator_t* iter, 
                                             lab_lexer_token_container_t* tokens, 
                                             void* user_data);

extern bool              numeric_callback_rule(char c) { return (isdigit(c) > 0); }
extern lab_lexer_token_t numeric_callback(const lab_vec_t* code,
                                          lab_lexer_iterator_t* iter, 
                                          lab_lexer_token_container_t* tokens, 
                                          void* user_data);

extern bool              symbol_callback_rule(char c) {  }
extern lab_lexer_token_t symbol_callback(const lab_vec_t* code,
                                         lab_lexer_iterator_t* iter, 
                                         lab_lexer_token_container_t* tokens, 
                                         void* user_data);

extern bool              operator_callback_rule(char c);
extern lab_lexer_token_t operator_callback(const lab_vec_t* code,
                                           lab_lexer_iterator_t* iter, 
                                           lab_lexer_token_container_t* tokens, 
                                           void* user_data);

extern bool              string_callback_rule(char c) { return (c == '\"' || c == '\''); }
extern lab_lexer_token_t string_callback(const lab_vec_t* code,
                                         lab_lexer_iterator_t* iter, 
                                         lab_lexer_token_container_t* tokens, 
                                         void* user_data);

extern bool              eof_callback_rule(char c) { return (c == '\0'); }
extern lab_lexer_token_t eof_callback(const lab_vec_t* code,
                                      lab_lexer_iterator_t* iter, 
                                      lab_lexer_token_container_t* tokens, 
                                      void* user_data);