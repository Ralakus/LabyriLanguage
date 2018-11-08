#include "lexer.h"
#include <stdbool.h>

#include <stdint.h>

typedef struct lab_parser_token_container_t {

    size_t             token_count;
    lab_lexer_token_t* tokens;
    size_t             iter;

} lab_parser_token_container_t;

typedef enum lab_parser_ast_expr_kind_e {

    lab_expr_num,
    lab_expr_var,
    lab_expr_bin,
    lab_expr_call,

} lab_parser_ast_expr_kind_e;

typedef struct lab_parser_ast_expr_t {

    void*                      expr;
    lab_parser_ast_expr_kind_e kind;

} lab_parser_ast_expr_t;

typedef struct lab_parser_ast_expr_num_t {
    
    bool is_float;
    union {
        double  float_val;
        int64_t int_val;
    };

} lab_parser_ast_expr_num_t;

typedef struct lab_parser_ast_expr_var_t {

    char* name;

} lab_parser_ast_expr_var_t;

typedef struct lab_parser_ast_expr_bin_t {
    
    int op;
    lab_parser_ast_expr_t* lhs, *rhs;

} lab_parser_ast_expr_bin_t;

typedef struct lab_parser_ast_expr_call_t {
    char* callee;
    size_t argc;
    lab_parser_ast_expr_t* args;

} lab_parser_ast_expr_call_t;

