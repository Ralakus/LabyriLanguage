#pragma once

#include <lab/mempool.h>

typedef enum lab_ast_expr_kind_e_t {

    LAB_AST_EXPR_KIND_NUM,
    LAB_AST_EXPR_KIND_VAR,
    LAB_AST_EXPR_KIND_BIN,
    LAB_AST_EXPR_KIND_CALL,

} lab_ast_expr_kind_e_t;

typedef struct lab_ast_expr_t {

    void* expr;
    lab_ast_expr_kind_e_t kind;

    struct lab_ast_expr_t* next;

} lab_ast_expr_t;

extern lab_ast_expr_t* lab_ast_expr_make(lab_mempool_t* pool, void* expr, lab_ast_expr_kind_e_t kind);

typedef struct lab_ast_expr_num_t {
    
    bool is_float;
    union {
        size_t integer;
        double floating;
    } as;

} lab_ast_expr_num_t;

extern lab_ast_expr_num_t* lab_ast_expr_num_make_float(lab_mempool_t* pool, float  val);
extern lab_ast_expr_num_t* lab_ast_expr_num_make_int  (lab_mempool_t* pool, size_t val);

typedef struct lab_ast_expr_var_t {

    char* name;

} lab_ast_expr_var_t;

extern lab_ast_expr_var_t* lab_ast_expr_var_make(lab_mempool_t* pool, const char* name, size_t name_len);

typedef struct lab_ast_expr_bin_t {

    char op;             // Operator
    lab_ast_expr_t* lhs;
    lab_ast_expr_t* rhs;

} lab_ast_expr_bin_t;

extern lab_ast_expr_bin_t* lab_ast_expr_bin_make(lab_mempool_t* pool, char op, lab_ast_expr_t* lhs, lab_ast_expr_t* rhs);

typedef struct lab_ast_expr_call_t {

    char* callee;
    size_t argc;
    lab_ast_expr_t* args;

} lab_ast_expr_call_t;

extern lab_ast_expr_call_t* lab_ast_expr_call_make(lab_mempool_t* pool, char* callee, size_t name_len, lab_ast_expr_t* args, size_t argc);