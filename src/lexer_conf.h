#pragma once

#include <lab/logger.h>
#include "lexer.h"
#include "tokens.h"

extern char* tok_to_string(lab_tokens_e tok);

extern bool              alpha_callback_rule(char c);
extern lab_lexer_token_t alpha_callback(const lab_vec_t* code,
                                        lab_lexer_iterator_t* iter, 
                                        lab_lexer_token_container_t* tokens, 
                                        void* user_data);

extern bool              whitespace_callback_rule(char c);
extern lab_lexer_token_t whitespace_callback(const lab_vec_t* code,
                                             lab_lexer_iterator_t* iter, 
                                             lab_lexer_token_container_t* tokens, 
                                             void* user_data);

extern bool              numeric_callback_rule(char c);
extern lab_lexer_token_t numeric_callback(const lab_vec_t* code,
                                          lab_lexer_iterator_t* iter, 
                                          lab_lexer_token_container_t* tokens, 
                                          void* user_data);

extern bool              symbol_callback_rule(char c);
extern lab_lexer_token_t symbol_callback(const lab_vec_t* code,
                                         lab_lexer_iterator_t* iter, 
                                         lab_lexer_token_container_t* tokens, 
                                         void* user_data);

extern bool              operator_callback_rule(char c);
extern lab_lexer_token_t operator_callback(const lab_vec_t* code,
                                           lab_lexer_iterator_t* iter, 
                                           lab_lexer_token_container_t* tokens, 
                                           void* user_data);

extern bool              string_callback_rule(char c);
extern lab_lexer_token_t string_callback(const lab_vec_t* code,
                                         lab_lexer_iterator_t* iter, 
                                         lab_lexer_token_container_t* tokens, 
                                         void* user_data);

extern bool              eof_callback_rule(char c);
extern lab_lexer_token_t eof_callback(const lab_vec_t* code,
                                      lab_lexer_iterator_t* iter, 
                                      lab_lexer_token_container_t* tokens, 
                                      void* user_data);