#pragma once

#include <lab/vector.h>

typedef struct lab_lexer_token_t {

    int id;
    char* data;
    size_t line;
    size_t column;

} lab_lexer_token_t;

typedef lab_vec_t lab_lexer_token_container_t;

typedef struct lab_lexer_iterator_t {
    size_t iter;
    size_t line;
    size_t column;
} lab_lexer_iterator_t;

typedef bool (*lab_lexer_callback_t)(const lab_vec_t* code,
                                     lab_lexer_iterator_t* iter, 
                                     lab_lexer_token_container_t* tokens, 
                                     void* user_data);

typedef bool (*lab_lexer_rule_callback_t)(char c);

typedef struct lab_lexer_rule_t {

    lab_lexer_rule_callback_t rule_function;
    lab_lexer_callback_t      callback;

} lab_lexer_rule_t;

typedef lab_vec_t lab_lexer_ruleset_t;

/*
    Rulesets must be initilized before use and freed afterwards
    rule_count in init can be left at 0, what it does is preallocate the amount of rules
*/
extern bool lab_lexer_ruleset_init(lab_lexer_ruleset_t* ruleset,
                                   size_t rule_count);
extern void lab_lexer_ruleset_free(lab_lexer_ruleset_t* ruleset);
extern bool lab_lexer_ruleset_add_rule(lab_lexer_ruleset_t*      ruleset,
                                       lab_lexer_rule_callback_t rule_function,
                                       lab_lexer_callback_t      callback);

/*
    Token containers must be initilized before use and freed afterwards
*/
extern bool lab_lexer_token_container_init(lab_lexer_token_container_t* container);
extern void lab_lexer_token_container_free(lab_lexer_token_container_t* container);

/*
    Adds new token
*/
extern bool lab_lexer_token_container_append(lab_lexer_token_container_t* container,
                                             const lab_vec_t* code,
                                             size_t iter,
                                             int id, 
                                             char* data, 
                                             size_t line, 
                                             size_t column);

/*
    Lexes code
*/
extern bool lab_lexer_lex(lab_lexer_token_container_t* tokens, 
                          lab_lexer_ruleset_t* ruleset, 
                          const lab_vec_t* code, 
                          void* user_data);

/*
    Automatically gets new line and column position while iterating
*/
extern void lab_lexer_iter_next(const lab_vec_t* code, 
                                lab_lexer_iterator_t* iter);
extern void lab_lexer_iter_prev(const lab_vec_t* code, 
                                lab_lexer_iterator_t* iter);