#pragma once

#include <lab/logger.h>
#include "lexer.h"
#include "tokens.h"

extern char* tok_to_string(lab_tokens_e_t tok);

extern bool alpha_callback_rule(char c);
extern bool alpha_callback(const lab_vec_t* code,
                           lab_lexer_iterator_t* iter, 
                           lab_lexer_token_container_t* tokens, 
                           void* user_data);

extern bool whitespace_callback_rule(char c);
extern bool whitespace_callback(const lab_vec_t* code,
                                lab_lexer_iterator_t* iter, 
                                lab_lexer_token_container_t* tokens, 
                                void* user_data);

extern bool numeric_callback_rule(char c);
extern bool numeric_callback(const lab_vec_t* code,
                             lab_lexer_iterator_t* iter, 
                             lab_lexer_token_container_t* tokens, 
                             void* user_data);

extern bool symbol_callback_rule(char c);
extern bool symbol_callback(const lab_vec_t* code,
                            lab_lexer_iterator_t* iter, 
                            lab_lexer_token_container_t* tokens, 
                            void* user_data);

extern bool operator_callback_rule(char c);
extern bool operator_callback(const lab_vec_t* code,
                              lab_lexer_iterator_t* iter, 
                              lab_lexer_token_container_t* tokens, 
                              void* user_data);

extern bool string_callback_rule(char c);
extern bool string_callback(const lab_vec_t* code,
                            lab_lexer_iterator_t* iter, 
                            lab_lexer_token_container_t* tokens, 
                            void* user_data);

extern bool eof_callback_rule(char c);
extern bool eof_callback(const lab_vec_t* code,
                         lab_lexer_iterator_t* iter, 
                         lab_lexer_token_container_t* tokens, 
                         void* user_data);

extern bool lab_custom_lexer_lex(lab_lexer_token_container_t* tokens, 
                                 const lab_vec_t* code, 
                                 void* user_data);